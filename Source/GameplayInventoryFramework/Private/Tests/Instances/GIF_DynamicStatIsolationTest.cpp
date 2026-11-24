#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Instances/InventoryItemInstance.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFDynamicStatIsolationTest, "GameplayInventory.Instances.DynamicStatIsolation", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFDynamicStatIsolationTest::RunTest(const FString& Parameters)
{
	UInventoryItemInstance* InstanceA = NewObject<UInventoryItemInstance>(GetTransientPackage());
	UInventoryItemInstance* InstanceB = NewObject<UInventoryItemInstance>(GetTransientPackage());

	TestNotNull(TEXT("Instance A created"), InstanceA);
	TestNotNull(TEXT("Instance B created"), InstanceB);

	if (!InstanceA || !InstanceB)
	{
		return false;
	}

	// 1. Float Parameter: Durability
	const FGameplayTag DurabilityTag = MGITags::Item_Stat_Durability;
	InstanceA->SetParamFloat(DurabilityTag, 100.0f);
	InstanceB->SetParamFloat(DurabilityTag, 100.0f);

	// Modify Instance A only
	InstanceA->SetParamFloat(DurabilityTag, 45.0f);
	TestEqual(TEXT("Instance A durability updated to 45.0"), InstanceA->GetParamFloat(DurabilityTag), 45.0f);
	TestEqual(TEXT("Instance B durability remains 100.0"), InstanceB->GetParamFloat(DurabilityTag), 100.0f);

	// 2. Int Parameter: Current Ammo and Reserve Ammo
	const FGameplayTag AmmoTag = MGITags::Item_Stat_Ammo;
	const FGameplayTag ReserveAmmoTag = MGITags::Item_Stat_Ammo_Reserve;

	InstanceA->SetParamInt(AmmoTag, 30);
	InstanceA->SetParamInt(ReserveAmmoTag, 120);

	InstanceB->SetParamInt(AmmoTag, 15);
	InstanceB->SetParamInt(ReserveAmmoTag, 45);

	TestEqual(TEXT("Instance A current ammo is 30"), InstanceA->GetParamInt(AmmoTag), 30);
	TestEqual(TEXT("Instance A reserve ammo is 120"), InstanceA->GetParamInt(ReserveAmmoTag), 120);
	TestEqual(TEXT("Instance B current ammo is 15"), InstanceB->GetParamInt(AmmoTag), 15);
	TestEqual(TEXT("Instance B reserve ammo is 45"), InstanceB->GetParamInt(ReserveAmmoTag), 45);

	// 3. Bool Parameter: IsSilenced
	const FGameplayTag SilencedParamTag = MGITags::Item_Param_IsSilenced;
	InstanceA->SetParamBool(SilencedParamTag, true);
	InstanceB->SetParamBool(SilencedParamTag, false);

	TestTrue(TEXT("Instance A is silenced"), InstanceA->GetParamBool(SilencedParamTag));
	TestFalse(TEXT("Instance B is not silenced"), InstanceB->GetParamBool(SilencedParamTag));

	// 4. String Parameter: Crafter Name
	const FGameplayTag CrafterTag = MGITags::Item_Param_Crafter;
	InstanceA->SetParamString(CrafterTag, TEXT("MasterBlacksmith"));

	TestEqual(TEXT("Instance A crafter name set"), InstanceA->GetParamString(CrafterTag), FString(TEXT("MasterBlacksmith")));
	TestEqual(TEXT("Instance B crafter name empty default"), InstanceB->GetParamString(CrafterTag), FString(TEXT("")));

	// 5. Tag Container Parameter: Installed Attachments
	const FGameplayTag AttachmentsTag = MGITags::Item_Param_Attachments;
	FGameplayTagContainer Mods;
	Mods.AddTag(MGITags::Item_Mod_Scope_ACOG);
	Mods.AddTag(MGITags::Item_Mod_Barrel_Suppressor);
	InstanceA->SetParamTagContainer(AttachmentsTag, Mods);

	TestTrue(TEXT("Instance A has attachments container"), InstanceA->HasParameter(AttachmentsTag));
	FGameplayTagContainer ReadMods = InstanceA->GetParamTagContainer(AttachmentsTag);
	TestEqual(TEXT("Instance A has 2 attachments"), ReadMods.Num(), 2);
	TestEqual(TEXT("Instance B has 0 attachments"), InstanceB->GetParamTagContainer(AttachmentsTag).Num(), 0);

	// 6. Parameter Removal
	TestTrue(TEXT("RemoveParameter returns true for existing parameter"), InstanceA->RemoveParameter(CrafterTag));
	TestFalse(TEXT("Parameter is removed"), InstanceA->HasParameter(CrafterTag));

	// 7. Dynamic Tag Isolation
	const FGameplayTag JammedTag = MGITags::Item_State_Jammed;
	InstanceA->AddDynamicTag(JammedTag);

	TestTrue(TEXT("Instance A is tagged Jammed"), InstanceA->HasDynamicTag(JammedTag));
	TestFalse(TEXT("Instance B must NOT be Jammed"), InstanceB->HasDynamicTag(JammedTag));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
