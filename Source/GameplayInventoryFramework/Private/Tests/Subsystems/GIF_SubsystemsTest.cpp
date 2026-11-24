#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Subsystems/ItemRegistrySubsystem.h"
#include "Subsystems/WorldItemSubsystem.h"
#include "World/ItemComponent.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_UIInfo.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFSubsystemsTest, "GameplayInventory.Subsystems.RegistryAndSpatialStreaming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFSubsystemsTest::RunTest(const FString& Parameters)
{
	// ==========================================
	// 1. Test UItemRegistrySubsystem (GameInstance Subsystem)
	// ==========================================
	UGameInstance* GameInstance = NewObject<UGameInstance>(GetTransientPackage());
	TestNotNull(TEXT("GameInstance must be valid"), GameInstance);
	if (!GameInstance)
	{
		return false;
	}
	GameInstance->Init();

	UItemRegistrySubsystem* RegistrySubsystem = GameInstance->GetSubsystem<UItemRegistrySubsystem>();
	TestNotNull(TEXT("ItemRegistrySubsystem must be valid"), RegistrySubsystem);
	if (RegistrySubsystem)
	{
		// Create mock item definition
		UInventoryItemDefinition* TestDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
		TestDef->ItemIdentifierTag = MGITags::Item_Type_Weapon;

		UFragment_UIInfo* UIFrag = NewObject<UFragment_UIInfo>(TestDef);
		UIFrag->FragmentTag = MGITags::Item_Fragment_UI;
		UIFrag->DisplayName = FText::FromString(TEXT("Excalibur"));
		TestDef->AddFragment(UIFrag);

		// Register item definition
		RegistrySubsystem->RegisterItemDefinition(TestDef);

		// Lookup by tag
		const UInventoryItemDefinition* RetrievedDef = RegistrySubsystem->GetItemDefinition(MGITags::Item_Type_Weapon);
		TestNotNull(TEXT("Item definition retrieved by tag"), RetrievedDef);
		TestEqual(TEXT("Retrieved definition matches registered"), RetrievedDef, static_cast<const UInventoryItemDefinition*>(TestDef));

		// Missing tag returns nullptr safely
		const FGameplayTag NonExistentTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Type.NonExistent"), false);
		const UInventoryItemDefinition* MissingDef = RegistrySubsystem->GetItemDefinition(NonExistentTag);
		TestNull(TEXT("Non-existent tag lookup returns nullptr"), MissingDef);

		// Async handle testing (empty soft paths executes callback directly)
		bool bLoadedCallbackFired = false;
		RegistrySubsystem->RequestAsyncLoadItemAssets(TestDef, FStreamableDelegate::CreateLambda([&]()
		{
			bLoadedCallbackFired = true;
		}));
		TestTrue(TEXT("Async load callback executed"), bLoadedCallbackFired);

		// Release assets
		RegistrySubsystem->ReleaseItemAssets(TestDef);
		TestFalse(TEXT("Active handle released"), RegistrySubsystem->HasActiveStreamableHandle(TestDef));
	}

	// ==========================================
	// 2. Test UWorldItemSubsystem (World Subsystem)
	// ==========================================
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	UWorldItemSubsystem* WorldSubsystem = World->GetSubsystem<UWorldItemSubsystem>();
	TestNotNull(TEXT("WorldItemSubsystem must be valid"), WorldSubsystem);
	if (WorldSubsystem)
	{
		// Spawn World Item 1 at (100, 0, 0)
		AActor* ItemActor1 = World->SpawnActor<AActor>();
		USceneComponent* Root1 = NewObject<USceneComponent>(ItemActor1);
		ItemActor1->SetRootComponent(Root1);
		Root1->RegisterComponent();
		ItemActor1->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
		UItemComponent* ItemComp1 = NewObject<UItemComponent>(ItemActor1);
		ItemComp1->SetPayload(nullptr, 5, nullptr);
		ItemActor1->AddInstanceComponent(ItemComp1);
		ItemComp1->RegisterComponent();

		// Spawn World Item 2 at (500, 0, 0)
		AActor* ItemActor2 = World->SpawnActor<AActor>();
		USceneComponent* Root2 = NewObject<USceneComponent>(ItemActor2);
		ItemActor2->SetRootComponent(Root2);
		Root2->RegisterComponent();
		ItemActor2->SetActorLocation(FVector(500.0f, 0.0f, 0.0f));
		UItemComponent* ItemComp2 = NewObject<UItemComponent>(ItemActor2);
		ItemComp2->SetPayload(nullptr, 1, nullptr);
		ItemActor2->AddInstanceComponent(ItemComp2);
		ItemComp2->RegisterComponent();

		// Manually register if BeginPlay didn't run in transient world
		WorldSubsystem->RegisterWorldItem(ItemComp1);
		WorldSubsystem->RegisterWorldItem(ItemComp2);

		TestEqual(TEXT("Registered item count matches"), WorldSubsystem->GetRegisteredItemCount(), 2);

		// Proximity Query 1: Radius 200 around Origin -> Only Item 1 is within range
		TArray<UItemComponent*> Nearby1 = WorldSubsystem->FindNearbyItems(FVector::ZeroVector, 200.0f);
		TestEqual(TEXT("Nearby items within radius 200"), Nearby1.Num(), 1);
		if (Nearby1.Num() == 1)
		{
			TestEqual(TEXT("Item 1 found"), Nearby1[0], ItemComp1);
		}

		// Proximity Query 2: Radius 600 around Origin -> Both items within range
		TArray<UItemComponent*> Nearby2 = WorldSubsystem->FindNearbyItems(FVector::ZeroVector, 600.0f);
		TestEqual(TEXT("Nearby items within radius 600"), Nearby2.Num(), 2);

		// Proximity Query 3: Radius 50 around Origin -> Zero items
		TArray<UItemComponent*> Nearby3 = WorldSubsystem->FindNearbyItems(FVector::ZeroVector, 50.0f);
		TestEqual(TEXT("Nearby items within radius 50 is empty"), Nearby3.Num(), 0);

		// Unregister Item 1
		WorldSubsystem->UnregisterWorldItem(ItemComp1);
		TestEqual(TEXT("Registered count decreased after unregister"), WorldSubsystem->GetRegisteredItemCount(), 1);

		// Cleanup actors
		ItemActor1->Destroy();
		ItemActor2->Destroy();
	}

	World->DestroyWorld(false);
	GameInstance->Shutdown();

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
