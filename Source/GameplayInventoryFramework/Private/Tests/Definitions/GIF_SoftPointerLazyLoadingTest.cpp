#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_Montage.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFSoftPointerLazyLoadingTest, "GameplayInventory.Definitions.SoftPointerLazyLoading", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFSoftPointerLazyLoadingTest::RunTest(const FString& Parameters)
{
	UInventoryItemDefinition* ItemDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	TestNotNull(TEXT("ItemDef must be valid"), ItemDef);

	UFragment_Montage* MontageFrag = NewObject<UFragment_Montage>(ItemDef);
	
	// Assign a soft object path without loading asset into memory
	const FSoftObjectPath SoftAnimPath(TEXT("/Game/Test/NonExistentMontage.NonExistentMontage"));
	FItemMontageEntry Entry;
	Entry.AnimMontage = TSoftObjectPtr<UAnimMontage>(SoftAnimPath);
	MontageFrag->Montages.Add(Entry);
	ItemDef->AddFragment(MontageFrag);

	// Verify soft pointer points to path without forcing synchronous disk load
	TestEqual(TEXT("Soft object path matches"), MontageFrag->Montages[0].AnimMontage.ToSoftObjectPath(), SoftAnimPath);
	TestFalse(TEXT("Asset must not be synchronously loaded into memory"), MontageFrag->Montages[0].AnimMontage.IsValid());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
