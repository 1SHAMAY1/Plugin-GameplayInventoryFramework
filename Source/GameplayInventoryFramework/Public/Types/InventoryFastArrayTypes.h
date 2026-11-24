#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Instances/InventoryItemInstance.h"
#include "InventoryFastArrayTypes.generated.h"

struct FInventoryList;
class UActorComponent;

/**
 * A single entry in the fast array replicated inventory.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryEntry()
		: Instance(nullptr)
		, StackCount(0)
	{
	}

	FInventoryEntry(UInventoryItemInstance* InInstance, int32 InStackCount)
		: Instance(InInstance)
		, StackCount(InStackCount)
	{
	}

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryItemInstance> Instance;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 StackCount;

	/** Serialization and replication lifecycle callbacks */
	void PreReplicatedRemove(const FInventoryList& InArraySerializer);
	void PostReplicatedAdd(const FInventoryList& InArraySerializer);
	void PostReplicatedChange(const FInventoryList& InArraySerializer);

	FString GetDebugString() const;
};

/**
 * High-performance replicated inventory list using Unreal Engine's FastArraySerializer.
 * Sends delta diffs (only added, removed, or changed items) rather than the whole array.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> Entries;

	void SetOwnerComponent(UActorComponent* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}

	UActorComponent* GetOwnerComponent() const
	{
		return OwnerComponent.Get();
	}

	/** Adds an entry and marks it dirty for network replication */
	FInventoryEntry& AddEntry(UInventoryItemInstance* Instance, int32 StackCount);

	/** Modifies an existing entry's stack count and marks dirty */
	bool ModifyEntryStack(int32 EntryIndex, int32 NewStackCount);

	/** Removes an entry at index and marks array dirty */
	bool RemoveEntry(int32 EntryIndex);

	/** Finds the first entry matching the item instance */
	int32 FindEntryIndexByInstance(const UInventoryItemInstance* Instance) const;

	/** Clears all entries */
	void Empty();

	/** FastArraySerializer delta serialization implementation */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this);
	}

	void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

private:
	friend struct FInventoryEntry;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
