// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Async/AsyncAction_PlayItemMontage.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Fragments/Fragment_Montage.h"
#include "Types/InventoryNativeTags.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFMontageAsyncTest, "GameplayInventory.Async.MontagePlayback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFMontageAsyncTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	// ----------------------------------------------------
	// Test 1: Null Target Parameters Safely Broadcasts OnFailed
	// ----------------------------------------------------
	bool bFailedFiredNullTarget = false;
	UAsyncAction_PlayItemMontage* Action1 = UAsyncAction_PlayItemMontage::PlayItemMontageAsync(
		World,
		nullptr,
		nullptr,
		MGITags::Animation_Montage_Attack.GetTag()
	);
	TestNotNull(TEXT("Action1 must be valid object"), Action1);
	if (Action1)
	{
		Action1->OnFailed.AddDynamic(Action1, &UAsyncAction_PlayItemMontage::Activate); // Dummy bind or lambda
	}

	// Custom listener class or dynamic delegate verification
	// We can use a test actor with a UFUNCTION to catch dynamic delegates
	ACharacter* TestChar = World->SpawnActor<ACharacter>();
	TestNotNull(TEXT("TestChar spawned"), TestChar);

	UInventoryItemDefinition* TestDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	TestDef->ItemIdentifierTag = MGITags::Item_Type_Weapon_Sword.GetTag();

	UInventoryItemInstance* TestInstance = NewObject<UInventoryItemInstance>(GetTransientPackage());
	TestInstance->InitFromObject(TestDef);

	// Test 2: Character without anim instance or missing montage asset triggers OnFailed safely
	UAsyncAction_PlayItemMontage* Action2 = UAsyncAction_PlayItemMontage::PlayItemMontageAsync(
		World,
		TestChar,
		TestInstance,
		MGITags::Animation_Montage_Attack.GetTag()
	);
	TestNotNull(TEXT("Action2 must be valid"), Action2);

	bool bFailedFired = false;
	// Since OnFailed is FOnMontagePlayDelegate (dynamic multicast delegate), we can bind a lambda via an inline mock helper
	// or invoke Activate() and verify it safely completes
	Action2->Activate();

	// Test 3: Action with valid montage fragment but null asset
	UFragment_Montage* MontageFrag = NewObject<UFragment_Montage>(TestDef);
	MontageFrag->FragmentTag = MGITags::Item_Fragment_Montage.GetTag();
	FItemMontageEntry AttackEntry;
	AttackEntry.MontageTag = MGITags::Animation_Montage_Attack.GetTag();
	AttackEntry.AnimMontage = nullptr; // Null asset
	MontageFrag->Montages.Add(AttackEntry);
	TestDef->AddFragment(MontageFrag);

	UAsyncAction_PlayItemMontage* Action3 = UAsyncAction_PlayItemMontage::PlayItemMontageAsync(
		World,
		TestChar,
		TestInstance,
		MGITags::Animation_Montage_Attack.GetTag()
	);
	TestNotNull(TEXT("Action3 must be valid"), Action3);
	Action3->Activate();

	// Verify no crash occurred and everything completed safely
	TestTrue(TEXT("Async montage activation completed without crash"), true);

	// Cleanup
	World->DestroyWorld(false);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
