// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "FunctionLibraries/InventoryFunctionLibrary.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Fragments/Fragment_Attachment.h"
#include "Fragments/Fragment_Montage.h"
#include "Fragments/Fragment_UIInfo.h"
#include "Subsystems/ItemRegistrySubsystem.h"
#include "Subsystems/WorldItemSubsystem.h"
#include "Types/InventoryNativeTags.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFFunctionLibraryTest, "GameplayInventory.FunctionLibrary.HelpersAndQueries",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFFunctionLibraryTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	// ----------------------------------------------------
	// 1. Test Static Fragment Queries (GetFragmentByTag, HasFragmentTag)
	// ----------------------------------------------------
	UInventoryItemDefinition* TestDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	TestDef->ItemIdentifierTag = MGITags::Item_Type_Weapon_Sword.GetTag();

	UFragment_UIInfo* UIFrag = NewObject<UFragment_UIInfo>(TestDef);
	UIFrag->FragmentTag = MGITags::Item_Fragment_UI.GetTag();
	UIFrag->DisplayName = FText::FromString(TEXT("Iron Longsword"));
	TestDef->AddFragment(UIFrag);

	TestTrue(TEXT("HasFragmentTag returns true for UI fragment"),
		UInventoryFunctionLibrary::HasFragmentTag(TestDef, MGITags::Item_Fragment_UI.GetTag()));
	TestFalse(TEXT("HasFragmentTag returns false for non-existent fragment"),
		UInventoryFunctionLibrary::HasFragmentTag(TestDef, MGITags::Item_Fragment_Audio.GetTag()));

	const UInventoryItemFragment* RetrievedFrag = UInventoryFunctionLibrary::GetFragmentByTag(TestDef, MGITags::Item_Fragment_UI.GetTag());
	TestNotNull(TEXT("Retrieved fragment must be non-null"), RetrievedFrag);
	TestTrue(TEXT("Retrieved fragment matches UIFrag"), RetrievedFrag == UIFrag);

	// ----------------------------------------------------
	// 2. Test Montage Query Helper (GetMontageByTag)
	// ----------------------------------------------------
	UFragment_Montage* MontageFrag = NewObject<UFragment_Montage>(TestDef);
	MontageFrag->FragmentTag = MGITags::Item_Fragment_Montage.GetTag();

	FItemMontageEntry AttackEntry;
	AttackEntry.MontageTag = MGITags::Animation_Montage_Attack.GetTag();
	AttackEntry.PlayRate = 1.25f;
	AttackEntry.StartingSection = FName(TEXT("Combo1"));
	MontageFrag->Montages.Add(AttackEntry);
	TestDef->AddFragment(MontageFrag);

	FItemMontageEntry FoundEntry;
	bool bFoundMontage = UInventoryFunctionLibrary::GetMontageByTag(TestDef, MGITags::Animation_Montage_Attack.GetTag(), FoundEntry);
	TestTrue(TEXT("GetMontageByTag succeeded for Attack"), bFoundMontage);
	TestEqual(TEXT("Montage PlayRate matches"), FoundEntry.PlayRate, 1.25f);
	TestEqual(TEXT("Montage StartingSection matches"), FoundEntry.StartingSection, FName(TEXT("Combo1")));

	FItemMontageEntry MissingEntry;
	bool bMissingMontage = UInventoryFunctionLibrary::GetMontageByTag(TestDef, MGITags::Animation_Montage_Reload.GetTag(), MissingEntry);
	TestFalse(TEXT("GetMontageByTag returns false for unconfigured reload"), bMissingMontage);

	// ----------------------------------------------------
	// 3. Test Dynamic Stat Accessors (Float, Int, Bool)
	// ----------------------------------------------------
	UInventoryItemInstance* TestInstance = NewObject<UInventoryItemInstance>(GetTransientPackage());
	TestInstance->InitFromObject(TestDef);

	// Float stat
	UInventoryFunctionLibrary::SetItemDynamicFloat(TestInstance, MGITags::Item_Stat_Durability.GetTag(), 85.5f);
	float Durability = UInventoryFunctionLibrary::GetItemDynamicFloat(TestInstance, MGITags::Item_Stat_Durability.GetTag(), 100.0f);
	TestEqual(TEXT("Dynamic float stat matches"), Durability, 85.5f);

	// Int stat
	UInventoryFunctionLibrary::SetItemDynamicInt(TestInstance, MGITags::Item_Stat_Ammo.GetTag(), 30);
	int32 Ammo = UInventoryFunctionLibrary::GetItemDynamicInt(TestInstance, MGITags::Item_Stat_Ammo.GetTag(), 0);
	TestEqual(TEXT("Dynamic int stat matches"), Ammo, 30);

	// Bool stat
	UInventoryFunctionLibrary::SetItemDynamicBool(TestInstance, MGITags::Item_Param_IsSilenced.GetTag(), true);
	bool bSilenced = UInventoryFunctionLibrary::GetItemDynamicBool(TestInstance, MGITags::Item_Param_IsSilenced.GetTag(), false);
	TestTrue(TEXT("Dynamic bool stat matches"), bSilenced);

	// Fallback default value test
	float FallbackWeight = UInventoryFunctionLibrary::GetItemDynamicFloat(TestInstance, MGITags::Item_Stat_Weight.GetTag(), 12.3f);
	TestEqual(TEXT("Fallback default float returned for missing stat"), FallbackWeight, 12.3f);

	// ----------------------------------------------------
	// 4. Test Visual Attachment & Detachment
	// ----------------------------------------------------
	UFragment_Attachment* AttachFrag = NewObject<UFragment_Attachment>(TestDef);
	AttachFrag->FragmentTag = MGITags::Item_Fragment_Attachment.GetTag();
	AttachFrag->SocketName = FName(TEXT("RightHandSocket"));
	AttachFrag->bSpawnAsActor = false;
	AttachFrag->bIsSkeletalMesh = false;
	TestDef->AddFragment(AttachFrag);

	AActor* CharacterActor = World->SpawnActor<AActor>();
	USceneComponent* CharRoot = NewObject<USceneComponent>(CharacterActor, TEXT("RootComponent"));
	CharacterActor->SetRootComponent(CharRoot);
	CharRoot->RegisterComponent();

	USceneComponent* AttachedComp = UInventoryFunctionLibrary::AttachItemVisual(CharRoot, TestInstance);
	TestNotNull(TEXT("Attached component must be non-null"), AttachedComp);
	TestTrue(TEXT("Attached component is child of CharRoot"), AttachedComp && AttachedComp->GetAttachParent() == CharRoot);
	TestEqual(TEXT("Attached socket name matches"), AttachedComp->GetAttachSocketName(), FName(TEXT("RightHandSocket")));

	// Detach
	UInventoryFunctionLibrary::DetachItemVisual(CharRoot, FName(TEXT("RightHandSocket")));
	TestEqual(TEXT("Children array empty after detachment"), CharRoot->GetAttachChildren().Num(), 0);

	// ----------------------------------------------------
	// 5. Subsystem Query Accessors
	// ----------------------------------------------------
	UWorldItemSubsystem* WorldSub = UInventoryFunctionLibrary::GetWorldItemSubsystem(World);
	TestNotNull(TEXT("GetWorldItemSubsystem returns valid subsystem"), WorldSub);

	// Cleanup
	World->DestroyWorld(false);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
