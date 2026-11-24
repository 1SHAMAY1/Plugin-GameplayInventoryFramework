#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Components/InventoryComponent.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_Stacking.h"
#include "Fragments/Fragment_ItemStats.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFInventoryCapacityTest, "GameplayInventory.Components.InventoryCapacityAndFilters", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFInventoryCapacityTest::RunTest(const FString& Parameters)
{
	UInventoryComponent* Inv = NewObject<UInventoryComponent>(GetTransientPackage());
	TestNotNull(TEXT("Inventory component created"), Inv);
	if (!Inv)
	{
		return false;
	}

	// 1. Tag-Based Dynamic Capacity Limits Configuration
	Inv->SetCapacityLimit(MGITags::Inventory_Capacity_Slots, 2.0f);
	Inv->SetCapacityLimit(MGITags::Inventory_Capacity_Weight, 50.0f);

	TestEqual(TEXT("GetCapacityLimit for Slots is 2.0"), Inv->GetCapacityLimit(MGITags::Inventory_Capacity_Slots), 2.0f);
	TestEqual(TEXT("GetMaxSlots convenience method returns 2"), Inv->GetMaxSlots(), 2);
	TestEqual(TEXT("GetMaxWeight convenience method returns 50.0"), Inv->GetMaxWeight(), 50.0f);
	TestTrue(TEXT("HasCapacityLimit for Slots is true"), Inv->HasCapacityLimit(MGITags::Inventory_Capacity_Slots));

	// 2. Custom Game-Specific Capacity Limit (e.g. Volume)
	const FGameplayTag VolumeLimitTag = MGITags::Inventory_Capacity_Volume;
	Inv->SetCapacityLimit(VolumeLimitTag, 10.0f);
	TestTrue(TEXT("Has custom Volume capacity limit"), Inv->HasCapacityLimit(VolumeLimitTag));
	TestEqual(TEXT("Volume limit is 10.0"), Inv->GetCapacityLimit(VolumeLimitTag), 10.0f);

	// 3. Tag Query Filter: Only accept Potions
	Inv->SetAllowedTagQuery(FGameplayTagQuery::MakeQuery_MatchTag(MGITags::Item_Type_Consumable_Potion));

	// Test A: Reject weapon on tag mismatch
	UInventoryItemDefinition* WeaponDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	WeaponDef->ItemIdentifierTag = MGITags::Item_Type_Weapon_Sword;
	TestFalse(TEXT("Weapon rejected by potion filter"), Inv->CanAcceptItem(WeaponDef, 1));

	// Test B: Accept potion 1 (Slot 1) with UFragment_Stacking
	UInventoryItemDefinition* PotionDef1 = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	PotionDef1->ItemIdentifierTag = MGITags::Item_Type_Consumable_Potion;
	UFragment_Stacking* StackFrag1 = NewObject<UFragment_Stacking>(PotionDef1);
	StackFrag1->MaxStackCount = 1;
	PotionDef1->AddFragment(StackFrag1);

	TestTrue(TEXT("Slot 1 accepted"), Inv->CanAcceptItem(PotionDef1, 1));
	TestTrue(TEXT("Slot 1 added"), Inv->TryAddItemDefinitionObject(PotionDef1, 1));
	TestEqual(TEXT("Inventory entry count is 1"), Inv->GetEntryCount(), 1);
	TestEqual(TEXT("Usage for slots is 1.0"), Inv->GetCurrentUsageForLimit(MGITags::Inventory_Capacity_Slots), 1.0f);

	// Test C: Add second potion (Slot 2)
	UInventoryItemDefinition* PotionDef2 = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	PotionDef2->ItemIdentifierTag = MGITags::Item_Type_Consumable_Potion;
	UFragment_Stacking* StackFrag2 = NewObject<UFragment_Stacking>(PotionDef2);
	StackFrag2->MaxStackCount = 1;
	PotionDef2->AddFragment(StackFrag2);

	TestTrue(TEXT("Slot 2 accepted"), Inv->CanAcceptItem(PotionDef2, 1));
	TestTrue(TEXT("Slot 2 added"), Inv->TryAddItemDefinitionObject(PotionDef2, 1));
	TestEqual(TEXT("Inventory entry count is 2"), Inv->GetEntryCount(), 2);
	TestEqual(TEXT("Usage for slots is 2.0"), Inv->GetCurrentUsageForLimit(MGITags::Inventory_Capacity_Slots), 2.0f);

	// Test D: Reject third potion because Slots limit is 2
	UInventoryItemDefinition* PotionDef3 = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	PotionDef3->ItemIdentifierTag = MGITags::Item_Type_Consumable_Potion;
	UFragment_Stacking* StackFrag3 = NewObject<UFragment_Stacking>(PotionDef3);
	StackFrag3->MaxStackCount = 1;
	PotionDef3->AddFragment(StackFrag3);

	TestFalse(TEXT("Slot 3 rejected due to slot capacity"), Inv->CanAcceptItem(PotionDef3, 1));
	TestFalse(TEXT("Slot 3 add rejected"), Inv->TryAddItemDefinitionObject(PotionDef3, 1));
	TestEqual(TEXT("Inventory entry count remains 2"), Inv->GetEntryCount(), 2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
