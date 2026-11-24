// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "World/ItemComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SceneComponent.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Mass/MassItemFragments.h"
#include "Types/InventoryNativeTags.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFWorldLootLifecycleTest, "GameplayInventory.World.WorldLootLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFWorldLootLifecycleTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	// 1. Setup Mock Inventory Actor & Inventory Component
	AActor* CharacterActor = World->SpawnActor<AActor>();
	USceneComponent* CharRoot = NewObject<USceneComponent>(CharacterActor);
	CharacterActor->SetRootComponent(CharRoot);
	CharRoot->RegisterComponent();

	UInventoryComponent* InvComp = NewObject<UInventoryComponent>(CharacterActor);
	CharacterActor->AddInstanceComponent(InvComp);
	InvComp->RegisterComponent();
	InvComp->SetMaxSlots(10);

	// 2. Setup Mock Item Definition
	UInventoryItemDefinition* ItemDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	ItemDef->ItemIdentifierTag = MGITags::Item_Type_Resource;
	ItemDef->MaxStackCount = 20;

	// ----------------------------------------------------
	// Test 1: InitFromDefinition & Pickup Execution Lifecycle
	// ----------------------------------------------------
	AActor* LootActor1 = World->SpawnActor<AActor>();
	USceneComponent* LootRoot1 = NewObject<USceneComponent>(LootActor1);
	LootActor1->SetRootComponent(LootRoot1);
	LootRoot1->RegisterComponent();

	UItemComponent* ItemComp1 = NewObject<UItemComponent>(LootActor1);
	LootActor1->AddInstanceComponent(ItemComp1);
	ItemComp1->RegisterComponent();
	ItemComp1->InitFromDefinition(ItemDef, 5);

	// Verify Initial State
	TestFalse(TEXT("Loot actor is initially visible"), LootActor1->IsHidden());
	TestFalse(TEXT("ItemComp is initially not looted"), ItemComp1->IsLooted());
	TestEqual(TEXT("Payload StackCount matches"), ItemComp1->GetPayload().StackCount, 5);
	TestTrue(TEXT("Payload ItemDef matches"), ItemComp1->GetPayload().ItemDef.Get() == ItemDef);

	// Bind to Native Lifecycle Delegates to verify 3-stage firing order
	TArray<FString> FiredEvents;
	ItemComp1->OnPreItemAddedNative.AddLambda([&](UInventoryComponent* Target)
	{
		FiredEvents.Add(TEXT("PreItemAdded"));
	});
	ItemComp1->OnItemAddedNative.AddLambda([&](UInventoryComponent* Target, UInventoryItemInstance* Inst)
	{
		FiredEvents.Add(TEXT("ItemAdded"));
	});
	ItemComp1->OnPostItemAddedNative.AddLambda([&](UInventoryComponent* Target, UInventoryItemInstance* Inst)
	{
		FiredEvents.Add(TEXT("PostItemAdded"));
	});

	// Execute Pickup
	bool bPickedUp = ItemComp1->ExecutePickup(InvComp);
	TestTrue(TEXT("ExecutePickup succeeded"), bPickedUp);
	TestTrue(TEXT("ItemComp is marked as looted"), ItemComp1->IsLooted());
	TestTrue(TEXT("Loot actor is hidden after pickup"), LootActor1->IsHidden());
	TestFalse(TEXT("Loot actor collision disabled"), LootActor1->GetActorEnableCollision());

	// Verify Delegate Lifecycle Order
	TestEqual(TEXT("Exactly 3 lifecycle events fired"), FiredEvents.Num(), 3);
	if (FiredEvents.Num() == 3)
	{
		TestEqual(TEXT("Stage 1 is PreItemAdded"), FiredEvents[0], TEXT("PreItemAdded"));
		TestEqual(TEXT("Stage 2 is ItemAdded"), FiredEvents[1], TEXT("ItemAdded"));
		TestEqual(TEXT("Stage 3 is PostItemAdded"), FiredEvents[2], TEXT("PostItemAdded"));
	}

	// Verify Inventory state
	TestEqual(TEXT("Target inventory received 5 items"), InvComp->GetTotalItemCount(), 5);

	// Repeated Pickup should fail (Idempotency)
	bool bDuplicatePickup = ItemComp1->ExecutePickup(InvComp);
	TestFalse(TEXT("Duplicate pickup must fail"), bDuplicatePickup);

	// ----------------------------------------------------
	// Test 2: InitFromInstance & ExtractItem
	// ----------------------------------------------------
	UInventoryItemInstance* DynamicInstance = NewObject<UInventoryItemInstance>(GetTransientPackage());
	DynamicInstance->InitFromObject(ItemDef);

	AActor* LootActor2 = World->SpawnActor<AActor>();
	USceneComponent* LootRoot2 = NewObject<USceneComponent>(LootActor2);
	LootActor2->SetRootComponent(LootRoot2);
	LootRoot2->RegisterComponent();

	UItemComponent* ItemComp2 = NewObject<UItemComponent>(LootActor2);
	LootActor2->AddInstanceComponent(ItemComp2);
	ItemComp2->RegisterComponent();
	ItemComp2->InitFromInstance(DynamicInstance, 3);

	UInventoryItemInstance* ExtractedInst = nullptr;
	int32 ExtractedCount = 0;
	bool bExtracted = ItemComp2->ExtractItem(ExtractedInst, ExtractedCount);

	TestTrue(TEXT("ExtractItem succeeded"), bExtracted);
	TestTrue(TEXT("Extracted instance matches"), ExtractedInst == DynamicInstance);
	TestEqual(TEXT("Extracted count matches"), ExtractedCount, 3);
	TestTrue(TEXT("ItemComp2 is marked as looted"), ItemComp2->IsLooted());
	TestTrue(TEXT("Loot actor 2 is hidden"), LootActor2->IsHidden());

	// Duplicate extract must fail
	UInventoryItemInstance* SecondExtractedInst = nullptr;
	int32 SecondExtractedCount = 0;
	bool bSecondExtract = ItemComp2->ExtractItem(SecondExtractedInst, SecondExtractedCount);
	TestFalse(TEXT("Duplicate ExtractItem must fail"), bSecondExtract);

	// ----------------------------------------------------
	// Test 3: Mass ECS Fragment & Tag Struct Validation
	// ----------------------------------------------------
	FMassItemDropFragment MassFragment;
	MassFragment.ItemTag = MGITags::Item_Type_Resource.GetTag();
	MassFragment.Quantity = 42;

	TestEqual(TEXT("Mass fragment quantity matches"), MassFragment.Quantity, 42);
	TestEqual(TEXT("Mass fragment item tag matches"), MassFragment.ItemTag, MGITags::Item_Type_Resource.GetTag());

	FMassItemDropTag MassTag;
	TestTrue(TEXT("Mass tag is zero payload archetype tag"), sizeof(MassTag) <= sizeof(FMassTag) + 8);

	// Cleanup
	World->DestroyWorld(false);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
