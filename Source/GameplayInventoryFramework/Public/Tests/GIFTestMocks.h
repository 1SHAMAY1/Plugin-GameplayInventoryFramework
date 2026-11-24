// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InventoryInterface.h"
#include "Interfaces/ItemInteractionInterface.h"
#include "Components/InventoryComponent.h"
#include "GIFTestMocks.generated.h"

/**
 * Mock Actor implementing IInventoryInterface and IItemInteractionInterface for test verification and decoupling validation.
 */
UCLASS()
class GAMEPLAYINVENTORYFRAMEWORK_API AMockInventoryActor : public AActor, public IInventoryInterface, public IItemInteractionInterface
{
	GENERATED_BODY()

public:
	AMockInventoryActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> MockInventory;

	// IInventoryInterface
	virtual UInventoryComponent* GetInventoryComponent_Implementation(FGameplayTag ContainerTag = FGameplayTag()) const override;
	virtual UInventoryManagerComponent* GetInventoryManagerComponent_Implementation() const override;

	// IItemInteractionInterface
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void GetInteractionPrompt_Implementation(FText& OutActionPrompt, FText& OutItemName, int32& OutCount) const override;
	virtual bool ExecuteInteract_Implementation(AActor* Interactor) override;

	bool bWasInteracted = false;
};
