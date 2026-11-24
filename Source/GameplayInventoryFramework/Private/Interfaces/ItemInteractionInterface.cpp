// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interfaces/ItemInteractionInterface.h"
#include "GameFramework/Actor.h"

bool IItemInteractionInterface::CanInteractWithActor(const AActor* TargetActor, AActor* Interactor)
{
	if (!TargetActor)
	{
		return false;
	}

	if (const IItemInteractionInterface* NativeInt = Cast<IItemInteractionInterface>(TargetActor))
	{
		return NativeInt->CanInteract_Implementation(Interactor);
	}

	if (TargetActor->GetClass()->ImplementsInterface(UItemInteractionInterface::StaticClass()))
	{
		return IItemInteractionInterface::Execute_CanInteract(TargetActor, Interactor);
	}

	return false;
}

bool IItemInteractionInterface::InteractWithActor(AActor* TargetActor, AActor* Interactor)
{
	if (!TargetActor)
	{
		return false;
	}

	if (IItemInteractionInterface* NativeInt = Cast<IItemInteractionInterface>(TargetActor))
	{
		return NativeInt->ExecuteInteract_Implementation(Interactor);
	}

	if (TargetActor->GetClass()->ImplementsInterface(UItemInteractionInterface::StaticClass()))
	{
		return IItemInteractionInterface::Execute_ExecuteInteract(TargetActor, Interactor);
	}

	return false;
}
