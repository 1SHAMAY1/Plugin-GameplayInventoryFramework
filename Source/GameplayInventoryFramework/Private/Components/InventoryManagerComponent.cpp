#include "Components/InventoryManagerComponent.h"
#include "Components/InventoryComponent.h"
#include "Instances/InventoryItemInstance.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Types/InventoryNativeTags.h"
#include "GameFramework/Actor.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Auto-discover and register all inventory components attached to the owning actor
	if (AActor* OwnerActor = GetOwner())
	{
		TArray<UInventoryComponent*> OwnerInventories;
		OwnerActor->GetComponents<UInventoryComponent>(OwnerInventories);
		for (UInventoryComponent* InvComp : OwnerInventories)
		{
			RegisterInventory(InvComp);
		}
	}
}

void UInventoryManagerComponent::RegisterInventory(UInventoryComponent* InventoryComp)
{
	if (InventoryComp && !RegisteredInventories.Contains(InventoryComp))
	{
		RegisteredInventories.Add(InventoryComp);
	}
}

void UInventoryManagerComponent::UnregisterInventory(UInventoryComponent* InventoryComp)
{
	if (InventoryComp)
	{
		RegisteredInventories.Remove(InventoryComp);
	}
}

UInventoryComponent* UInventoryManagerComponent::FindInventoryByTag(FGameplayTag ContainerTag) const
{
	if (!ContainerTag.IsValid())
	{
		return nullptr;
	}

	for (const TWeakObjectPtr<UInventoryComponent>& WeakComp : RegisteredInventories)
	{
		if (UInventoryComponent* Comp = WeakComp.Get())
		{
			const FGameplayTag CompTag = Comp->GetContainerTypeTag();
			if (CompTag.MatchesTagExact(ContainerTag) || CompTag.MatchesTag(ContainerTag))
			{
				return Comp;
			}
		}
	}

	return nullptr;
}

bool UInventoryManagerComponent::TransferItem(UInventoryComponent* SourceInv, UInventoryComponent* TargetInv, UInventoryItemInstance* ItemInstance, int32 Count)
{
	if (!SourceInv || !TargetInv || !ItemInstance || Count <= 0 || SourceInv == TargetInv)
	{
		return false;
	}

	const int32 EntryIdx = SourceInv->InventoryList.FindEntryIndexByInstance(ItemInstance);
	if (EntryIdx == INDEX_NONE)
	{
		return false;
	}

	const FInventoryEntry& SourceEntry = SourceInv->InventoryList.Entries[EntryIdx];
	if (SourceEntry.StackCount < Count)
	{
		return false;
	}

	const UInventoryItemDefinition* ItemDef = ItemInstance->GetItemDef();
	if (!TargetInv->CanAcceptItem(ItemDef, Count))
	{
		return false;
	}

	// Server-authoritative atomic transfer
	if (SourceEntry.StackCount == Count)
	{
		SourceInv->RemoveItemInstance(ItemInstance, Count);
		TargetInv->TryAddItemInstance(ItemInstance, Count);
	}
	else
	{
		SourceInv->InventoryList.ModifyEntryStack(EntryIdx, SourceEntry.StackCount - Count);
		TargetInv->TryAddItem(ItemDef, Count);
	}

	OnItemTransferred.Broadcast(SourceInv, TargetInv, ItemInstance, Count);
	OnItemTransferredNative.Broadcast(SourceInv, TargetInv, ItemInstance, Count);
	return true;
}

bool UInventoryManagerComponent::SwapSlots(UInventoryComponent* SourceInv, int32 SourceIndex, UInventoryComponent* TargetInv, int32 TargetIndex)
{
	if (!SourceInv || !TargetInv)
	{
		return false;
	}

	// Intra-inventory swap
	if (SourceInv == TargetInv)
	{
		if (SourceIndex < 0 || SourceIndex >= SourceInv->GetEntryCount() ||
			TargetIndex < 0 || TargetIndex >= SourceInv->GetEntryCount())
		{
			return false;
		}

		if (SourceIndex == TargetIndex)
		{
			return true;
		}

		SourceInv->InventoryList.Entries.Swap(SourceIndex, TargetIndex);
		SourceInv->InventoryList.MarkItemDirty(SourceInv->InventoryList.Entries[SourceIndex]);
		SourceInv->InventoryList.MarkItemDirty(SourceInv->InventoryList.Entries[TargetIndex]);
		SourceInv->InventoryList.MarkArrayDirty();
		return true;
	}

	// Cross-inventory swap
	if (SourceIndex < 0 || SourceIndex >= SourceInv->GetEntryCount() ||
		TargetIndex < 0 || TargetIndex >= TargetInv->GetEntryCount())
	{
		return false;
	}

	const FInventoryEntry& SourceEntry = SourceInv->InventoryList.Entries[SourceIndex];
	const FInventoryEntry& TargetEntry = TargetInv->InventoryList.Entries[TargetIndex];

	const UInventoryItemDefinition* SourceDef = SourceEntry.Instance ? SourceEntry.Instance->GetItemDef() : nullptr;
	const UInventoryItemDefinition* TargetDef = TargetEntry.Instance ? TargetEntry.Instance->GetItemDef() : nullptr;

	if (!TargetInv->CanAcceptItem(SourceDef, SourceEntry.StackCount) ||
		!SourceInv->CanAcceptItem(TargetDef, TargetEntry.StackCount))
	{
		return false;
	}

	FInventoryEntry Temp = MoveTemp(SourceInv->InventoryList.Entries[SourceIndex]);
	SourceInv->InventoryList.Entries[SourceIndex] = MoveTemp(TargetInv->InventoryList.Entries[TargetIndex]);
	TargetInv->InventoryList.Entries[TargetIndex] = MoveTemp(Temp);

	SourceInv->InventoryList.MarkItemDirty(SourceInv->InventoryList.Entries[SourceIndex]);
	SourceInv->InventoryList.MarkArrayDirty();

	TargetInv->InventoryList.MarkItemDirty(TargetInv->InventoryList.Entries[TargetIndex]);
	TargetInv->InventoryList.MarkArrayDirty();

	return true;
}

bool UInventoryManagerComponent::SplitStack(UInventoryComponent* SourceInv, int32 SourceIndex, int32 SplitCount, UInventoryComponent* TargetInv)
{
	if (!SourceInv || !TargetInv || SplitCount <= 0)
	{
		return false;
	}

	if (SourceIndex < 0 || SourceIndex >= SourceInv->GetEntryCount())
	{
		return false;
	}

	const FInventoryEntry& SourceEntry = SourceInv->InventoryList.Entries[SourceIndex];
	if (SplitCount >= SourceEntry.StackCount)
	{
		return false;
	}

	const UInventoryItemDefinition* Def = SourceEntry.Instance ? SourceEntry.Instance->GetItemDef() : nullptr;
	if (!Def || !TargetInv->CanAcceptItem(Def, SplitCount))
	{
		return false;
	}

	SourceInv->InventoryList.ModifyEntryStack(SourceIndex, SourceEntry.StackCount - SplitCount);
	TargetInv->TryAddItem(Def, SplitCount);

	OnItemTransferred.Broadcast(SourceInv, TargetInv, SourceEntry.Instance, SplitCount);
	OnItemTransferredNative.Broadcast(SourceInv, TargetInv, SourceEntry.Instance, SplitCount);
	return true;
}

int32 UInventoryManagerComponent::LootAll(UInventoryComponent* SourceInv, UInventoryComponent* TargetInv)
{
	if (!SourceInv || !TargetInv || SourceInv == TargetInv)
	{
		return 0;
	}

	int32 TotalTransferred = 0;
	TArray<FInventoryEntry> EntriesToTransfer = SourceInv->GetAllEntries();
	for (const FInventoryEntry& Entry : EntriesToTransfer)
	{
		if (Entry.Instance && Entry.StackCount > 0)
		{
			if (TransferItem(SourceInv, TargetInv, Entry.Instance, Entry.StackCount))
			{
				TotalTransferred += Entry.StackCount;
			}
		}
	}

	return TotalTransferred;
}

bool UInventoryManagerComponent::AutoRouteItem(UInventoryItemInstance* Instance, int32 Count)
{
	if (!Instance || Count <= 0)
	{
		return false;
	}

	const UInventoryItemDefinition* Def = Instance->GetItemDef();
	if (!Def)
	{
		return false;
	}

	// 1. First priority: Check specialized containers with explicit tag filters
	for (const TWeakObjectPtr<UInventoryComponent>& WeakComp : RegisteredInventories)
	{
		if (UInventoryComponent* Comp = WeakComp.Get())
		{
			if (!Comp->AllowedItemFilter.IsEmpty() && Comp->CanAcceptItem(Def, Count))
			{
				return Comp->TryAddItemInstance(Instance, Count);
			}
		}
	}

	// 2. Second priority: Main container
	if (UInventoryComponent* MainInv = FindInventoryByTag(MGITags::Inventory_Type_Main))
	{
		if (MainInv->CanAcceptItem(Def, Count))
		{
			return MainInv->TryAddItemInstance(Instance, Count);
		}
	}

	// 3. Fallback: Any registered inventory with capacity
	for (const TWeakObjectPtr<UInventoryComponent>& WeakComp : RegisteredInventories)
	{
		if (UInventoryComponent* Comp = WeakComp.Get())
		{
			if (Comp->CanAcceptItem(Def, Count))
			{
				return Comp->TryAddItemInstance(Instance, Count);
			}
		}
	}

	return false;
}
