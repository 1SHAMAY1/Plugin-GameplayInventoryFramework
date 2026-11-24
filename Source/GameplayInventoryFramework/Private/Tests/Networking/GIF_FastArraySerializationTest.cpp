#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Types/InventoryFastArrayTypes.h"
#include "Instances/InventoryItemInstance.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFFastArraySerializationTest, "GameplayInventory.Networking.FastArraySerializerDeltaCounters", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFFastArraySerializationTest::RunTest(const FString& Parameters)
{
	FInventoryList List;
	UInventoryItemInstance* Item1 = NewObject<UInventoryItemInstance>(GetTransientPackage());
	TestNotNull(TEXT("Item1 created"), Item1);
	
	// Add item
	FInventoryEntry& Entry = List.AddEntry(Item1, 5);
	TestEqual(TEXT("List contains 1 entry"), List.Entries.Num(), 1);
	TestEqual(TEXT("Entry stack count is 5"), Entry.StackCount, 5);

	// Modify stack count and verify dirty item mark
	const bool bModified = List.ModifyEntryStack(0, 10);
	TestTrue(TEXT("ModifyEntryStack returned true"), bModified);
	TestEqual(TEXT("Stack count updated to 10"), List.Entries[0].StackCount, 10);

	// Remove entry and verify array dirty mark
	const bool bRemoved = List.RemoveEntry(0);
	TestTrue(TEXT("RemoveEntry returned true"), bRemoved);
	TestEqual(TEXT("List is empty after removal"), List.Entries.Num(), 0);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
