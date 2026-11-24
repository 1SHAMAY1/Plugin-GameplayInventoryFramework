// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tests/GIFTestMocks.h"

AMockInventoryActor::AMockInventoryActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MockInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("MockInventory"));
}

UInventoryComponent* AMockInventoryActor::GetInventoryComponent_Implementation(FGameplayTag ContainerTag) const
{
	return MockInventory;
}

UInventoryManagerComponent* AMockInventoryActor::GetInventoryManagerComponent_Implementation() const
{
	return nullptr;
}

bool AMockInventoryActor::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

void AMockInventoryActor::GetInteractionPrompt_Implementation(FText& OutActionPrompt, FText& OutItemName, int32& OutCount) const
{
	OutActionPrompt = FText::FromString(TEXT("Loot"));
	OutItemName = FText::FromString(TEXT("Treasure Chest"));
	OutCount = 1;
}

bool AMockInventoryActor::ExecuteInteract_Implementation(AActor* Interactor)
{
	bWasInteracted = true;
	return true;
}
