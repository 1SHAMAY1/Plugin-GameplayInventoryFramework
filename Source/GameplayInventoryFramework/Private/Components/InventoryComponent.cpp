#include "Components/InventoryComponent.h"
#include "Fragments/Fragment_ItemStats.h"
#include "Types/InventoryNativeTags.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UInventoryComponent::UInventoryComponent()
	: bReplicateToOwnerOnly(true)
	, InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	// Default dynamic capacity limit rules
	CapacityLimits.Add(FInventoryCapacityLimit(MGITags::Inventory_Capacity_Slots, 20.0f));
	CapacityLimits.Add(FInventoryCapacityLimit(MGITags::Inventory_Capacity_Weight, 100.0f));
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	InventoryList.SetOwnerComponent(this);
}

void UInventoryComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	InventoryList.SetOwnerComponent(this);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, CapacityLimits, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryList, COND_OwnerOnly);
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		UInventoryItemInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

// --- Dynamic Tag-Based Capacity Limits ---

void UInventoryComponent::SetCapacityLimit(FGameplayTag CapacityTag, float Value)
{
	if (!CapacityTag.IsValid())
	{
		return;
	}

	for (FInventoryCapacityLimit& Limit : CapacityLimits)
	{
		if (Limit.LimitTag == CapacityTag)
		{
			Limit.Value = Value;
			OnCapacityLimitChanged.Broadcast(CapacityTag, Value);
			if (CapacityTag == MGITags::Inventory_Capacity_Slots)
			{
				OnCapacityChanged.Broadcast(FMath::RoundToInt(Value));
			}
			else if (CapacityTag == MGITags::Inventory_Capacity_Weight)
			{
				OnWeightCapacityChanged.Broadcast(Value);
			}
			return;
		}
	}

	CapacityLimits.Add(FInventoryCapacityLimit(CapacityTag, Value));
	OnCapacityLimitChanged.Broadcast(CapacityTag, Value);

	if (CapacityTag == MGITags::Inventory_Capacity_Slots)
	{
		OnCapacityChanged.Broadcast(FMath::RoundToInt(Value));
	}
	else if (CapacityTag == MGITags::Inventory_Capacity_Weight)
	{
		OnWeightCapacityChanged.Broadcast(Value);
	}
}

float UInventoryComponent::GetCapacityLimit(FGameplayTag CapacityTag, float DefaultValue) const
{
	for (const FInventoryCapacityLimit& Limit : CapacityLimits)
	{
		if (Limit.LimitTag == CapacityTag)
		{
			return Limit.Value;
		}
	}
	return DefaultValue;
}

bool UInventoryComponent::HasCapacityLimit(FGameplayTag CapacityTag) const
{
	for (const FInventoryCapacityLimit& Limit : CapacityLimits)
	{
		if (Limit.LimitTag == CapacityTag)
		{
			return true;
		}
	}
	return false;
}

float UInventoryComponent::GetCurrentUsageForLimit(FGameplayTag CapacityTag) const
{
	if (CapacityTag == MGITags::Inventory_Capacity_Slots)
	{
		return static_cast<float>(InventoryList.Entries.Num());
	}
	if (CapacityTag == MGITags::Inventory_Capacity_Weight)
	{
		return CalculateCurrentWeight();
	}
	return 0.0f;
}

int32 UInventoryComponent::GetMaxSlots() const
{
	return FMath::RoundToInt(GetCapacityLimit(MGITags::Inventory_Capacity_Slots, 20.0f));
}

void UInventoryComponent::SetMaxSlots(int32 InMaxSlots)
{
	SetCapacityLimit(MGITags::Inventory_Capacity_Slots, static_cast<float>(FMath::Max(1, InMaxSlots)));
}

float UInventoryComponent::GetMaxWeight() const
{
	return GetCapacityLimit(MGITags::Inventory_Capacity_Weight, 100.0f);
}

void UInventoryComponent::SetMaxWeight(float InMaxWeight)
{
	SetCapacityLimit(MGITags::Inventory_Capacity_Weight, FMath::Max(0.0f, InMaxWeight));
}

void UInventoryComponent::OnRep_CapacityLimits()
{
	for (const FInventoryCapacityLimit& Limit : CapacityLimits)
	{
		OnCapacityLimitChanged.Broadcast(Limit.LimitTag, Limit.Value);
	}
	OnCapacityChanged.Broadcast(GetMaxSlots());
	OnWeightCapacityChanged.Broadcast(GetMaxWeight());
}

// --- Queries ---

int32 UInventoryComponent::GetTotalItemCount() const
{
	int32 Total = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		Total += Entry.StackCount;
	}
	return Total;
}

int32 UInventoryComponent::GetItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const
{
	if (!ItemDefClass)
	{
		return 0;
	}

	int32 Total = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.Instance && Entry.Instance->GetItemDefClass() == ItemDefClass)
		{
			Total += Entry.StackCount;
		}
	}
	return Total;
}

float UInventoryComponent::CalculateCurrentWeight() const
{
	float TotalWeight = 0.0f;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.Instance)
		{
			if (const UFragment_ItemStats* StatsFrag = Entry.Instance->FindFragmentByClass<UFragment_ItemStats>())
			{
				TotalWeight += (StatsFrag->Weight * Entry.StackCount);
			}
		}
	}
	return TotalWeight;
}

static bool DoesInstanceMatchDefinition(const UInventoryItemInstance* ExistingInst, const UInventoryItemDefinition* TargetDef)
{
	if (!ExistingInst || !TargetDef)
	{
		return false;
	}

	if (const UInventoryItemDefinition* ExistingDef = ExistingInst->GetItemDef())
	{
		if (ExistingDef == TargetDef)
		{
			return true;
		}
		if (ExistingDef->ItemIdentifierTag.IsValid() && TargetDef->ItemIdentifierTag.IsValid())
		{
			return ExistingDef->ItemIdentifierTag.MatchesTagExact(TargetDef->ItemIdentifierTag);
		}
		return false;
	}

	if (ExistingInst->GetItemDefClass() && ExistingInst->GetItemDefClass() != UInventoryItemDefinition::StaticClass())
	{
		return ExistingInst->GetItemDefClass() == TargetDef->GetClass();
	}

	return false;
}

static bool DoInstancesMatch(const UInventoryItemInstance* InstA, const UInventoryItemInstance* InstB)
{
	if (!InstA || !InstB)
	{
		return false;
	}

	const UInventoryItemDefinition* DefA = InstA->GetItemDef();
	const UInventoryItemDefinition* DefB = InstB->GetItemDef();
	if (DefA && DefB)
	{
		if (DefA == DefB)
		{
			return true;
		}
		if (DefA->ItemIdentifierTag.IsValid() && DefB->ItemIdentifierTag.IsValid())
		{
			return DefA->ItemIdentifierTag.MatchesTagExact(DefB->ItemIdentifierTag);
		}
		return false;
	}

	if (InstA->GetItemDefClass() && InstA->GetItemDefClass() != UInventoryItemDefinition::StaticClass())
	{
		return InstA->GetItemDefClass() == InstB->GetItemDefClass();
	}

	return false;
}

bool UInventoryComponent::CanAcceptItem(const UInventoryItemDefinition* ItemDef, int32 Count) const
{
	if (!ItemDef || Count <= 0)
	{
		return false;
	}

	// 1. Tag Query Filter
	if (!AllowedItemFilter.IsEmpty())
	{
		FGameplayTagContainer ItemTags;
		if (ItemDef->ItemIdentifierTag.IsValid())
		{
			ItemTags.AddTag(ItemDef->ItemIdentifierTag);
		}
		for (const TObjectPtr<UInventoryItemFragment>& Frag : ItemDef->Fragments)
		{
			if (Frag && Frag->FragmentTag.IsValid())
			{
				ItemTags.AddTag(Frag->FragmentTag);
			}
		}
		if (!AllowedItemFilter.Matches(ItemTags))
		{
			return false;
		}
	}

	// 2. Weight Capacity Check
	const float MaxWeightLimit = GetMaxWeight();
	if (MaxWeightLimit > 0.0f)
	{
		if (const UFragment_ItemStats* StatsFrag = ItemDef->FindFragmentByClass<UFragment_ItemStats>())
		{
			const float AdditionalWeight = StatsFrag->Weight * Count;
			if (CalculateCurrentWeight() + AdditionalWeight > MaxWeightLimit)
			{
				return false;
			}
		}
	}

	// 3. Slot Capacity Check (accounting for stackability via UFragment_Stacking)
	const int32 MaxStack = ItemDef->GetMaxStackCount();
	int32 RemainingCount = Count;

	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (DoesInstanceMatchDefinition(Entry.Instance, ItemDef))
		{
			const int32 AvailableInStack = MaxStack - Entry.StackCount;
			if (AvailableInStack > 0)
			{
				RemainingCount -= AvailableInStack;
				if (RemainingCount <= 0)
				{
					return true;
				}
			}
		}
	}

	// Needs new slot(s)
	const int32 NeededNewSlots = FMath::DivideAndRoundUp(RemainingCount, MaxStack);
	return (InventoryList.Entries.Num() + NeededNewSlots) <= GetMaxSlots();
}

bool UInventoryComponent::CanAcceptItemClass(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count) const
{
	if (!ItemDefClass)
	{
		return false;
	}
	return CanAcceptItem(GetDefault<UInventoryItemDefinition>(ItemDefClass), Count);
}

bool UInventoryComponent::GetEntryAtIndex(int32 Index, FInventoryEntry& OutEntry) const
{
	if (InventoryList.Entries.IsValidIndex(Index))
	{
		OutEntry = InventoryList.Entries[Index];
		return true;
	}
	return false;
}

UInventoryItemInstance* UInventoryComponent::FindFirstInstanceByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const
{
	if (!ItemDefClass)
	{
		return nullptr;
	}

	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.Instance && Entry.Instance->GetItemDefClass() == ItemDefClass)
		{
			return Entry.Instance;
		}
	}

	return nullptr;
}

TArray<UInventoryItemInstance*> UInventoryComponent::FindAllInstancesByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const
{
	TArray<UInventoryItemInstance*> Results;
	if (!ItemDefClass)
	{
		return Results;
	}

	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.Instance && Entry.Instance->GetItemDefClass() == ItemDefClass)
		{
			Results.Add(Entry.Instance);
		}
	}

	return Results;
}

bool UInventoryComponent::TryAddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count)
{
	if (!ItemDefClass || Count <= 0)
	{
		return false;
	}

	const UInventoryItemDefinition* ItemDef = GetDefault<UInventoryItemDefinition>(ItemDefClass);
	return TryAddItem(ItemDef, Count);
}

bool UInventoryComponent::TryAddItem(const UInventoryItemDefinition* ItemDef, int32 Count)
{
	if (!ItemDef || Count <= 0)
	{
		return false;
	}

	if (!CanAcceptItem(ItemDef, Count))
	{
		OnCapacityExceeded.Broadcast(ItemDef->GetClass());
		return false;
	}

	const int32 MaxStack = ItemDef->GetMaxStackCount();
	int32 RemainingToAdd = Count;

	// Fill existing stacks first
	for (int32 Index = 0; Index < InventoryList.Entries.Num(); ++Index)
	{
		FInventoryEntry& Entry = InventoryList.Entries[Index];
		if (Entry.Instance && DoesInstanceMatchDefinition(Entry.Instance, ItemDef) && Entry.StackCount < MaxStack)
		{
			const int32 SpaceInStack = MaxStack - Entry.StackCount;
			const int32 AddToStack = FMath::Min(RemainingToAdd, SpaceInStack);
			const int32 OldCount = Entry.StackCount;
			
			InventoryList.ModifyEntryStack(Index, OldCount + AddToStack);
			OnItemStackChanged.Broadcast(Entry.Instance, Entry.StackCount, OldCount);

			RemainingToAdd -= AddToStack;
			if (RemainingToAdd <= 0)
			{
				return true;
			}
		}
	}

	// Create new entries for leftover count
	while (RemainingToAdd > 0)
	{
		UObject* InstanceOuter = GetOwner() ? static_cast<UObject*>(GetOwner()) : static_cast<UObject*>(this);
		UInventoryItemInstance* NewInstance = NewObject<UInventoryItemInstance>(InstanceOuter);
		NewInstance->InitFromObject(ItemDef);

		const int32 StackForThisSlot = FMath::Min(RemainingToAdd, MaxStack);
		InventoryList.AddEntry(NewInstance, StackForThisSlot);
		OnItemAdded.Broadcast(NewInstance, StackForThisSlot);

		RemainingToAdd -= StackForThisSlot;
	}

	return true;
}

bool UInventoryComponent::TryAddItemInstance(UInventoryItemInstance* Instance, int32 Count)
{
	if (!Instance || Count <= 0)
	{
		return false;
	}

	const UInventoryItemDefinition* ItemDef = Instance->GetItemDef();
	if (!CanAcceptItem(ItemDef, Count))
	{
		OnCapacityExceeded.Broadcast(Instance->GetItemDefClass());
		return false;
	}

	const int32 MaxStack = ItemDef ? ItemDef->GetMaxStackCount() : 1;

	// If stackable and matches an existing instance of the exact same class and tags
	if (MaxStack > 1)
	{
		int32 Remaining = Count;
		for (int32 Index = 0; Index < InventoryList.Entries.Num(); ++Index)
		{
			FInventoryEntry& Entry = InventoryList.Entries[Index];
			if (Entry.Instance && DoInstancesMatch(Entry.Instance, Instance) && Entry.StackCount < MaxStack)
			{
				const int32 Space = MaxStack - Entry.StackCount;
				const int32 AddCount = FMath::Min(Remaining, Space);
				const int32 OldCount = Entry.StackCount;

				InventoryList.ModifyEntryStack(Index, OldCount + AddCount);
				OnItemStackChanged.Broadcast(Entry.Instance, Entry.StackCount, OldCount);

				Remaining -= AddCount;
				if (Remaining <= 0)
				{
					return true;
				}
			}
		}

		if (Remaining > 0 && InventoryList.Entries.Num() < GetMaxSlots())
		{
			InventoryList.AddEntry(Instance, Remaining);
			OnItemAdded.Broadcast(Instance, Remaining);
			return true;
		}

		return false;
	}

	// Non-stackable (Weapons/Equipment with unique instance data)
	if (InventoryList.Entries.Num() < GetMaxSlots())
	{
		InventoryList.AddEntry(Instance, 1);
		OnItemAdded.Broadcast(Instance, 1);
		return true;
	}

	return false;
}

bool UInventoryComponent::RemoveItemInstance(UInventoryItemInstance* Instance, int32 Count)
{
	if (!Instance || Count <= 0)
	{
		return false;
	}

	const int32 EntryIndex = InventoryList.FindEntryIndexByInstance(Instance);
	if (EntryIndex == INDEX_NONE)
	{
		return false;
	}

	FInventoryEntry& Entry = InventoryList.Entries[EntryIndex];
	if (Entry.StackCount <= Count)
	{
		const int32 RemovedCount = Entry.StackCount;
		InventoryList.RemoveEntry(EntryIndex);
		OnItemRemoved.Broadcast(Instance, RemovedCount);
	}
	else
	{
		const int32 OldCount = Entry.StackCount;
		InventoryList.ModifyEntryStack(EntryIndex, OldCount - Count);
		OnItemStackChanged.Broadcast(Instance, Entry.StackCount, OldCount);
	}

	return true;
}

int32 UInventoryComponent::RemoveItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count)
{
	if (!ItemDefClass || Count <= 0)
	{
		return 0;
	}

	int32 RemainingToRemove = Count;

	for (int32 Index = InventoryList.Entries.Num() - 1; Index >= 0; --Index)
	{
		FInventoryEntry& Entry = InventoryList.Entries[Index];
		if (Entry.Instance && Entry.Instance->GetItemDefClass() == ItemDefClass)
		{
			UInventoryItemInstance* Instance = Entry.Instance;
			if (Entry.StackCount <= RemainingToRemove)
			{
				const int32 Removed = Entry.StackCount;
				RemainingToRemove -= Removed;
				InventoryList.RemoveEntry(Index);
				OnItemRemoved.Broadcast(Instance, Removed);
			}
			else
			{
				const int32 OldCount = Entry.StackCount;
				InventoryList.ModifyEntryStack(Index, OldCount - RemainingToRemove);
				OnItemStackChanged.Broadcast(Instance, Entry.StackCount, OldCount);
				RemainingToRemove = 0;
			}

			if (RemainingToRemove <= 0)
			{
				break;
			}
		}
	}

	return Count - RemainingToRemove;
}

void UInventoryComponent::ClearInventory()
{
	for (int32 Index = InventoryList.Entries.Num() - 1; Index >= 0; --Index)
	{
		if (UInventoryItemInstance* Instance = InventoryList.Entries[Index].Instance)
		{
			OnItemRemoved.Broadcast(Instance, InventoryList.Entries[Index].StackCount);
		}
	}
	InventoryList.Empty();
}
