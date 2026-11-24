#include "World/ItemComponent.h"
#include "Subsystems/WorldItemSubsystem.h"
#include "Components/InventoryComponent.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent, Payload);
	DOREPLIFETIME(UItemComponent, bIsLooted);
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UWorldItemSubsystem* Subsystem = World->GetSubsystem<UWorldItemSubsystem>())
		{
			Subsystem->RegisterWorldItem(this);
		}
	}
}

void UItemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UWorldItemSubsystem* Subsystem = World->GetSubsystem<UWorldItemSubsystem>())
		{
			Subsystem->UnregisterWorldItem(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UItemComponent::InitFromInstance(UInventoryItemInstance* InInstance, int32 InCount)
{
	Payload.ItemInstance = InInstance;
	Payload.StackCount = InCount;
	Payload.ItemDef = InInstance ? InInstance->GetItemDef() : nullptr;
	Payload.ItemDefClass = InInstance ? InInstance->GetItemDefClass() : nullptr;
	bIsLooted = false;
}

void UItemComponent::InitFromDefinition(const UInventoryItemDefinition* InDef, int32 InCount)
{
	Payload.ItemDef = InDef;
	Payload.ItemDefClass = InDef ? InDef->GetClass() : nullptr;
	Payload.StackCount = InCount;
	Payload.ItemInstance = nullptr;
	bIsLooted = false;
}

void UItemComponent::InitFromDefinitionClass(TSubclassOf<UInventoryItemDefinition> InDefClass, int32 InCount)
{
	Payload.ItemDefClass = InDefClass;
	Payload.ItemDef = InDefClass ? GetDefault<UInventoryItemDefinition>(InDefClass) : nullptr;
	Payload.StackCount = InCount;
	Payload.ItemInstance = nullptr;
	bIsLooted = false;
}

void UItemComponent::SetPayload(const UInventoryItemDefinition* InDef, int32 InCount, UInventoryItemInstance* InInstance)
{
	Payload.ItemDef = InDef;
	Payload.ItemDefClass = InDef ? InDef->GetClass() : nullptr;
	Payload.StackCount = InCount;
	Payload.ItemInstance = InInstance;
	bIsLooted = false;
}

bool UItemComponent::ExecutePickup(UInventoryComponent* TargetInventory)
{
	if (bIsLooted || !TargetInventory)
	{
		return false;
	}

	const UInventoryItemDefinition* Def = nullptr;
	if (Payload.ItemInstance)
	{
		Def = Payload.ItemInstance->GetItemDef();
	}
	else if (Payload.ItemDef)
	{
		Def = Payload.ItemDef;
	}
	else if (Payload.ItemDefClass)
	{
		Def = GetDefault<UInventoryItemDefinition>(Payload.ItemDefClass);
	}

	if (!Def || !TargetInventory->CanAcceptItem(Def, Payload.StackCount))
	{
		return false;
	}

	// 1. Pre-Added Lifecycle Callbacks
	OnPreItemAdded.Broadcast(TargetInventory);
	OnPreItemAddedNative.Broadcast(TargetInventory);

	// 2. Add to Target Inventory
	bool bAdded = false;
	if (Payload.ItemInstance)
	{
		bAdded = TargetInventory->TryAddItemInstance(Payload.ItemInstance, Payload.StackCount);
	}
	else
	{
		bAdded = TargetInventory->TryAddItem(Def, Payload.StackCount);
	}

	if (!bAdded)
	{
		return false;
	}

	// 3. Item Added Lifecycle Callbacks
	UInventoryItemInstance* ReportedInstance = Payload.ItemInstance;
	if (!ReportedInstance && TargetInventory && Def)
	{
		for (const FInventoryEntry& Entry : TargetInventory->GetAllEntries())
		{
			if (Entry.Instance && (Entry.Instance->GetItemDef() == Def || Entry.Instance->GetItemDefClass() == Def->GetClass()))
			{
				ReportedInstance = Entry.Instance;
				break;
			}
		}
	}

	OnItemAdded.Broadcast(TargetInventory, ReportedInstance);
	OnItemAddedNative.Broadcast(TargetInventory, ReportedInstance);

	// 4. Mark Looted and Hide/Disable Actor
	bIsLooted = true;
	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->SetActorHiddenInGame(true);
		OwnerActor->SetActorEnableCollision(false);
	}

	// 5. Post-Added Lifecycle Callbacks
	OnPostItemAdded.Broadcast(TargetInventory, ReportedInstance);
	OnPostItemAddedNative.Broadcast(TargetInventory, ReportedInstance);

	return true;
}

bool UItemComponent::ExtractItem(UInventoryItemInstance*& OutInstance, int32& OutCount)
{
	if (bIsLooted)
	{
		return false;
	}

	if (!Payload.ItemInstance)
	{
		const UInventoryItemDefinition* Def = Payload.ItemDef;
		if (!Def && Payload.ItemDefClass)
		{
			Def = GetDefault<UInventoryItemDefinition>(Payload.ItemDefClass);
		}

		if (Def)
		{
			UObject* Outer = GetOwner() ? static_cast<UObject*>(GetOwner()) : static_cast<UObject*>(this);
			Payload.ItemInstance = NewObject<UInventoryItemInstance>(Outer);
			Payload.ItemInstance->InitFromObject(Def);
		}
	}

	OutInstance = Payload.ItemInstance;
	OutCount = Payload.StackCount;
	bIsLooted = true;

	if (AActor* OwnerActor = GetOwner())
	{
		OwnerActor->SetActorHiddenInGame(true);
		OwnerActor->SetActorEnableCollision(false);
	}

	return true;
}

FVector UItemComponent::GetItemWorldLocation() const
{
	if (const AActor* OwnerActor = GetOwner())
	{
		return OwnerActor->GetActorLocation();
	}
	return FVector::ZeroVector;
}

bool UItemComponent::MatchesFilterQuery(const FGameplayTagQuery& FilterQuery) const
{
	if (FilterQuery.IsEmpty())
	{
		return true;
	}

	FGameplayTagContainer CombinedTags;

	const UInventoryItemDefinition* Def = nullptr;
	if (Payload.ItemInstance)
	{
		Def = Payload.ItemInstance->GetItemDef();
		CombinedTags.AppendTags(Payload.ItemInstance->GetDynamicTags());
	}
	else if (Payload.ItemDef)
	{
		Def = Payload.ItemDef;
	}
	else if (Payload.ItemDefClass)
	{
		Def = GetDefault<UInventoryItemDefinition>(Payload.ItemDefClass);
	}

	if (Def)
	{
		if (Def->ItemIdentifierTag.IsValid())
		{
			CombinedTags.AddTag(Def->ItemIdentifierTag);
		}

		for (const TObjectPtr<UInventoryItemFragment>& Fragment : Def->Fragments)
		{
			if (Fragment)
			{
				if (Fragment->FragmentTag.IsValid())
				{
					CombinedTags.AddTag(Fragment->FragmentTag);
				}
				CombinedTags.AppendTags(Fragment->AdditionalTags);
			}
		}
	}

	return FilterQuery.Matches(CombinedTags);
}

void UItemComponent::OnRep_Payload()
{
}

void UItemComponent::OnRep_IsLooted()
{
	if (bIsLooted)
	{
		if (AActor* OwnerActor = GetOwner())
		{
			OwnerActor->SetActorHiddenInGame(true);
			OwnerActor->SetActorEnableCollision(false);
		}
	}
}
