#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Types/InventoryTypes.h"
#include "Types/InventoryFastArrayTypes.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, UInventoryItemInstance*, Item, int32, StackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, UInventoryItemInstance*, Item, int32, StackCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemStackChanged, UInventoryItemInstance*, Item, int32, NewCount, int32, OldCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryCapacityChanged, int32, NewMaxSlots);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryWeightCapacityChanged, float, NewMaxWeight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryCapacityExceeded, TSubclassOf<UInventoryItemDefinition>, RejectedItemDef);

/**
 * Core container component for managing an inventory.
 * Can be added to Characters, PlayerStates, Chests, Vehicles, or World Drop actors.
 * Powered by FFastArraySerializer for delta-only network replication and zero-tick event dispatching.
 * Capacities are fully dynamic and rule-based via replicated Tag-Float CapacityLimits.
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	virtual void InitializeComponent() override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	// --- Configuration & Tag Identification ---

	/** Identifying tag for this container (e.g. Inventory.Type.Main, Inventory.Type.Equipment, Inventory.Type.Bank) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (Categories = "Inventory.Type"))
	FGameplayTag ContainerTypeTag;

	UFUNCTION(BlueprintPure, Category = "Inventory", meta = (Categories = "Inventory.Type"))
	FGameplayTag GetContainerTypeTag() const { return ContainerTypeTag; }

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (Categories = "Inventory.Type"))
	void SetContainerTypeTag(UPARAM(meta = (Categories = "Inventory.Type")) FGameplayTag InTag) { ContainerTypeTag = InTag; }

	/** Acceptance filter: If non-empty, only items matching this query can be stored here */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FGameplayTagQuery AllowedItemFilter;

	/** Whether this inventory replicates only to the owning client (COND_OwnerOnly) or to all simulated proxies */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory|Networking")
	bool bReplicateToOwnerOnly;

	// --- Dynamic Tag-Based Capacity Rules ---

	/** Sets or adds a capacity limit rule by gameplay tag */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Capacity", meta = (Categories = "Inventory.Capacity"))
	void SetCapacityLimit(UPARAM(meta = (Categories = "Inventory.Capacity")) FGameplayTag CapacityTag, float Value);

	/** Reads a capacity limit rule by gameplay tag */
	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity", meta = (Categories = "Inventory.Capacity"))
	float GetCapacityLimit(UPARAM(meta = (Categories = "Inventory.Capacity")) FGameplayTag CapacityTag, float DefaultValue = 0.0f) const;

	/** Checks whether a specific capacity rule tag is configured */
	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity", meta = (Categories = "Inventory.Capacity"))
	bool HasCapacityLimit(UPARAM(meta = (Categories = "Inventory.Capacity")) FGameplayTag CapacityTag) const;

	/** Returns the current usage value for a given capacity limit tag (Slots, Weight, etc.) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity", meta = (Categories = "Inventory.Capacity"))
	float GetCurrentUsageForLimit(UPARAM(meta = (Categories = "Inventory.Capacity")) FGameplayTag CapacityTag) const;

	// --- Convenience Backward-Compatible Accessors ---

	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity")
	int32 GetMaxSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Capacity")
	void SetMaxSlots(int32 InMaxSlots);

	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity")
	float GetMaxWeight() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Capacity")
	void SetMaxWeight(float InMaxWeight);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Filter")
	void SetAllowedTagQuery(const FGameplayTagQuery& InQuery) { AllowedItemFilter = InQuery; }

	// --- Core Queries ---

	/** Total number of unique occupied slots / entries */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	int32 GetEntryCount() const { return InventoryList.Entries.Num(); }

	/** Total count of items (sum of all entry stack counts) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	int32 GetTotalItemCount() const;

	/** Total sum of items matching a specific item definition */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	int32 GetItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const;

	/** Total weight of all items currently stored (pure C++ calculation) */
	float CalculateCurrentWeight() const;

	/** Reads current weight for encumbrance calculations (Blueprint Facing) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Capacity")
	float GetCurrentWeight() const { return CalculateCurrentWeight(); }

	/** Checks if this container can accept the given item definition and count without violating filters or capacity */
	UFUNCTION(BlueprintPure, Category = "Inventory|Validation")
	bool CanAcceptItem(const UInventoryItemDefinition* ItemDef, int32 Count = 1) const;

	/** Checks if this container can accept the given item definition class */
	UFUNCTION(BlueprintPure, Category = "Inventory|Validation")
	bool CanAcceptItemClass(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count = 1) const;

	/** Retrieves all entries (read-only copy) */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	const TArray<FInventoryEntry>& GetAllEntries() const { return InventoryList.Entries; }

	/** Retrieves a specific entry by index */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	bool GetEntryAtIndex(int32 Index, FInventoryEntry& OutEntry) const;

	/** Finds the first item instance matching definition class */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	UInventoryItemInstance* FindFirstInstanceByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const;

	/** Finds all item instances matching definition class */
	UFUNCTION(BlueprintPure, Category = "Inventory|Entries")
	TArray<UInventoryItemInstance*> FindAllInstancesByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass) const;

	// --- Authoritative Operations (Server Only) ---

	/** Primary Blueprint node: Adds items directly by definition DataAsset */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations", meta = (DisplayName = "Try Add Item"))
	bool TryAddItem(const UInventoryItemDefinition* ItemDef, int32 Count = 1);

	/** Adds an item by definition class, managing stack consolidation across available slots */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
	bool TryAddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count = 1);

	/** C++ backward-compatible forwarder for TryAddItem */
	bool TryAddItemDefinitionObject(const UInventoryItemDefinition* ItemDef, int32 Count = 1)
	{
		return TryAddItem(ItemDef, Count);
	}

	/** Adds an existing item instance, respecting max stacks and slot limits */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
	bool TryAddItemInstance(UInventoryItemInstance* Instance, int32 Count = 1);

	/** Removes an item instance from the inventory by count */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
	bool RemoveItemInstance(UInventoryItemInstance* Instance, int32 Count = 1);

	/** Removes items matching an item definition class by count */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
	int32 RemoveItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefClass, int32 Count = 1);

	/** Clears all items */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Operations")
	void ClearInventory();

	// --- Delegates ---

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnItemStackChanged OnItemStackChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnInventoryCapacityChanged OnCapacityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnInventoryWeightCapacityChanged OnWeightCapacityChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnInventoryCapacityExceeded OnCapacityExceeded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Delegates")
	FOnCapacityLimitChanged OnCapacityLimitChanged;

protected:
	/** Replicated Tag-Float capacity limit rules (Slots, Weight, Volume, etc.) */
	UPROPERTY(ReplicatedUsing = OnRep_CapacityLimits, EditDefaultsOnly, Category = "Inventory|Capacity")
	TArray<FInventoryCapacityLimit> CapacityLimits;

	UFUNCTION()
	void OnRep_CapacityLimits();

	/** High-performance fast array serialized item list */
	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	friend struct FInventoryEntry;
	friend struct FInventoryList;
	friend class UInventoryManagerComponent;
};
