#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/InventoryComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_Stacking.h"
#include "Fragments/Fragment_ItemStats.h"
#include "Instances/InventoryItemInstance.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFAtomicTransferTest, "GameplayInventory.Management.AtomicCrossInventoryTransfer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFAtomicTransferTest::RunTest(const FString& Parameters)
{
	// 1. Setup Actor and Components
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	AActor* TestActor = World->SpawnActor<AActor>();
	TestNotNull(TEXT("TestActor must be valid"), TestActor);

	UInventoryManagerComponent* Manager = NewObject<UInventoryManagerComponent>(TestActor);
	TestActor->AddInstanceComponent(Manager);
	Manager->RegisterComponent();

	UInventoryComponent* MainInv = NewObject<UInventoryComponent>(TestActor);
	MainInv->SetContainerTypeTag(MGITags::Inventory_Type_Main);
	MainInv->SetMaxSlots(10);
	TestActor->AddInstanceComponent(MainInv);
	MainInv->RegisterComponent();

	UInventoryComponent* BankInv = NewObject<UInventoryComponent>(TestActor);
	BankInv->SetContainerTypeTag(MGITags::Inventory_Type_Bank);
	BankInv->SetMaxSlots(5);
	TestActor->AddInstanceComponent(BankInv);
	BankInv->RegisterComponent();

	Manager->RegisterInventory(MainInv);
	Manager->RegisterInventory(BankInv);

	// 2. Verification: Tag Lookup
	UInventoryComponent* FoundMain = Manager->FindInventoryByTag(MGITags::Inventory_Type_Main);
	TestEqual(TEXT("FindInventoryByTag retrieves MainInv"), FoundMain, MainInv);

	UInventoryComponent* FoundBank = Manager->FindInventoryByTag(MGITags::Inventory_Type_Bank);
	TestEqual(TEXT("FindInventoryByTag retrieves BankInv"), FoundBank, BankInv);

	// 3. Create Item Definitions
	UInventoryItemDefinition* StackableDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	StackableDef->ItemIdentifierTag = MGITags::Item_Type_Resource;
	UFragment_Stacking* StackingFrag = NewObject<UFragment_Stacking>(StackableDef);
	StackingFrag->MaxStackCount = 50;
	StackableDef->AddFragment(StackingFrag);

	UInventoryItemDefinition* RareDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	RareDef->ItemIdentifierTag = MGITags::Item_Type_Weapon;

	// 4. Add Items to MainInv
	bool bAdded1 = MainInv->TryAddItem(StackableDef, 20);
	TestTrue(TEXT("Added 20 stackable items to Main"), bAdded1);

	bool bAdded2 = MainInv->TryAddItem(RareDef, 1);
	TestTrue(TEXT("Added 1 rare weapon to Main"), bAdded2);

	UInventoryItemInstance* StackableInstance = MainInv->GetAllEntries()[0].Instance;
	UInventoryItemInstance* RareInstance = MainInv->GetAllEntries()[1].Instance;
	TestNotNull(TEXT("StackableInstance must exist in Main"), StackableInstance);
	TestNotNull(TEXT("RareInstance must exist in Main"), RareInstance);

	// 5. Verification: Atomic TransferItem
	bool bDelegateFired = false;
	Manager->OnItemTransferredNative.AddLambda([&](UInventoryComponent* Src, UInventoryComponent* Tgt, UInventoryItemInstance* Item, int32 Count)
	{
		bDelegateFired = true;
	});

	bool bTransferSuccess = Manager->TransferItem(MainInv, BankInv, RareInstance, 1);
	TestTrue(TEXT("TransferItem succeeded from Main to Bank"), bTransferSuccess);
	TestTrue(TEXT("OnItemTransferred delegate fired"), bDelegateFired);
	TestEqual(TEXT("MainInv lost RareInstance"), MainInv->GetEntryCount(), 1);
	TestEqual(TEXT("BankInv gained RareInstance"), BankInv->GetEntryCount(), 1);

	// 6. Verification: Transaction Rollback on Capacity Violation
	// Restrict Bank to 1 slot (it currently has 1, so it is 100% full)
	BankInv->SetMaxSlots(1);

	// Attempt transfer to full bank: Must fail and rollback with zero mutations
	bool bFailedTransfer = Manager->TransferItem(MainInv, BankInv, StackableInstance, 5);
	TestFalse(TEXT("Transfer to full bank must fail"), bFailedTransfer);
	TestEqual(TEXT("MainInv count unchanged after failed transfer"), MainInv->GetEntryCount(), 1);
	TestEqual(TEXT("BankInv count unchanged after failed transfer"), BankInv->GetEntryCount(), 1);

	// 7. Expand Bank capacity and test SplitStack
	BankInv->SetMaxSlots(5);
	bool bSplitSuccess = Manager->SplitStack(MainInv, 0, 5, BankInv);
	TestTrue(TEXT("SplitStack succeeded"), bSplitSuccess);
	TestEqual(TEXT("MainInv remaining count after split"), MainInv->GetAllEntries()[0].StackCount, 15);
	TestEqual(TEXT("BankInv entry count after split"), BankInv->GetEntryCount(), 2);

	// 8. Verification: SwapSlots
	// Bank slot 0 is RareInstance (count 1), Bank slot 1 is Stackable (count 5)
	bool bIntraSwap = Manager->SwapSlots(BankInv, 0, BankInv, 1);
	TestTrue(TEXT("Intra-inventory SwapSlots succeeded"), bIntraSwap);

	// Cross-inventory SwapSlots: Swap Main slot 0 (Stackable count 15) with Bank slot 0 (Stackable count 5)
	bool bCrossSwap = Manager->SwapSlots(MainInv, 0, BankInv, 0);
	TestTrue(TEXT("Cross-inventory SwapSlots succeeded"), bCrossSwap);

	// 9. Verification: AutoRouteItem
	UInventoryComponent* EquipInv = NewObject<UInventoryComponent>(TestActor);
	EquipInv->SetContainerTypeTag(MGITags::Inventory_Type_Equipment);
	EquipInv->SetAllowedTagQuery(FGameplayTagQuery::MakeQuery_MatchTag(MGITags::Item_Type_Weapon));
	TestActor->AddInstanceComponent(EquipInv);
	EquipInv->RegisterComponent();
	Manager->RegisterInventory(EquipInv);

	UInventoryItemInstance* NewWeaponInstance = NewObject<UInventoryItemInstance>(GetTransientPackage());
	NewWeaponInstance->InitFromObject(RareDef);

	bool bRouted = Manager->AutoRouteItem(NewWeaponInstance, 1);
	TestTrue(TEXT("AutoRouteItem routed weapon to specialized EquipInv"), bRouted);
	TestEqual(TEXT("EquipInv has the routed weapon"), EquipInv->GetEntryCount(), 1);

	// Cleanup
	TestActor->Destroy();
	World->DestroyWorld(false);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
