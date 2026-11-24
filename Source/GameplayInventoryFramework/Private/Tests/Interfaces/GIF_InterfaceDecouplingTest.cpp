#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/ItemInteractionInterface.h"
#include "Components/InventoryComponent.h"
#include "Engine/World.h"
#include "Tests/GIFTestMocks.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFInterfaceDecouplingTest, "GameplayInventory.Interfaces.InterfaceDecoupling", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFInterfaceDecouplingTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be created"), World);
	if (!World)
	{
		return false;
	}

	AMockInventoryActor* MockActor = World->SpawnActor<AMockInventoryActor>();
	TestNotNull(TEXT("Mock actor spawned"), MockActor);
	if (!MockActor)
	{
		World->DestroyWorld(false);
		return false;
	}

	TestNotNull(TEXT("Mock inventory created"), MockActor->MockInventory.Get());

	// 1. Verify UHT interface reflection without concrete class casting
	TestTrue(TEXT("Actor implements UInventoryInterface via reflection"), 
		MockActor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()));
	TestTrue(TEXT("Actor implements UItemInteractionInterface via reflection"), 
		MockActor->GetClass()->ImplementsInterface(UItemInteractionInterface::StaticClass()));

	// 2. Direct C++ interface casting without knowing actor concrete class
	AActor* GenericActor = MockActor;
	IInventoryInterface* InvInterface = Cast<IInventoryInterface>(GenericActor);
	TestNotNull(TEXT("Generic actor cast to IInventoryInterface"), InvInterface);

	IItemInteractionInterface* InteractInterface = Cast<IItemInteractionInterface>(GenericActor);
	TestNotNull(TEXT("Generic actor cast to IItemInteractionInterface"), InteractInterface);

	// 3. Resolve inventory via static decoupled resolver
	UInventoryComponent* ResolvedInv = IInventoryInterface::ResolveInventory(GenericActor);
	TestNotNull(TEXT("Resolved inventory component via interface"), ResolvedInv);
	TestEqual(TEXT("Resolved component matches mock component"), ResolvedInv, MockActor->MockInventory.Get());

	// 4. Test interaction interface methods
	TestTrue(TEXT("Can interact with actor via interface"), 
		IItemInteractionInterface::CanInteractWithActor(GenericActor, nullptr));

	TestFalse(TEXT("bWasInteracted is initially false"), MockActor->bWasInteracted);
	TestTrue(TEXT("Execute interact via interface returns true"), 
		IItemInteractionInterface::InteractWithActor(GenericActor, nullptr));
	TestTrue(TEXT("bWasInteracted was set to true by interaction"), MockActor->bWasInteracted);

	World->DestroyActor(MockActor);
	World->DestroyWorld(false);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
