#include "Types/InventoryFastArrayTypes.h"

void FInventoryEntry::PreReplicatedRemove(const FInventoryList& InArraySerializer)
{
}

void FInventoryEntry::PostReplicatedAdd(const FInventoryList& InArraySerializer)
{
}

void FInventoryEntry::PostReplicatedChange(const FInventoryList& InArraySerializer)
{
}

FString FInventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("Instance: %s, StackCount: %d"), 
		Instance ? *Instance->GetName() : TEXT("None"), 
		StackCount);
}

FInventoryEntry& FInventoryList::AddEntry(UInventoryItemInstance* Instance, int32 StackCount)
{
	check(Instance);
	FInventoryEntry& NewEntry = Entries.Emplace_GetRef(Instance, StackCount);
	MarkItemDirty(NewEntry);
	return NewEntry;
}

bool FInventoryList::ModifyEntryStack(int32 EntryIndex, int32 NewStackCount)
{
	if (Entries.IsValidIndex(EntryIndex))
	{
		Entries[EntryIndex].StackCount = NewStackCount;
		MarkItemDirty(Entries[EntryIndex]);
		return true;
	}
	return false;
}

bool FInventoryList::RemoveEntry(int32 EntryIndex)
{
	if (Entries.IsValidIndex(EntryIndex))
	{
		Entries.RemoveAt(EntryIndex);
		MarkArrayDirty();
		return true;
	}
	return false;
}

int32 FInventoryList::FindEntryIndexByInstance(const UInventoryItemInstance* Instance) const
{
	if (!Instance)
	{
		return INDEX_NONE;
	}

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		if (Entries[Index].Instance == Instance)
		{
			return Index;
		}
	}

	return INDEX_NONE;
}

void FInventoryList::Empty()
{
	if (Entries.Num() > 0)
	{
		Entries.Empty();
		MarkArrayDirty();
	}
}

void FInventoryList::PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters)
{
}
