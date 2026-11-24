// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interfaces/InventoryInterface.h"
#include "Components/InventoryComponent.h"
#include "Components/InventoryManagerComponent.h"
#include "GameFramework/Actor.h"

UInventoryComponent* IInventoryInterface::ResolveInventory(const AActor* Actor, FGameplayTag ContainerTag)
{
	if (!Actor)
	{
		return nullptr;
	}

	// 1. Direct C++ Native Interface Check (Fastest, zero overhead)
	if (const IInventoryInterface* NativeInt = Cast<IInventoryInterface>(Actor))
	{
		return NativeInt->GetInventoryComponent_Implementation(ContainerTag);
	}

	// 2. Blueprint Interface Check
	if (Actor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		return IInventoryInterface::Execute_GetInventoryComponent(Actor, ContainerTag);
	}

	// 3. Fallback direct component search
	return Actor->FindComponentByClass<UInventoryComponent>();
}

UInventoryManagerComponent* IInventoryInterface::ResolveInventoryManager(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	// 1. Direct C++ Native Interface Check
	if (const IInventoryInterface* NativeInt = Cast<IInventoryInterface>(Actor))
	{
		return NativeInt->GetInventoryManagerComponent_Implementation();
	}

	// 2. Blueprint Interface Check
	if (Actor->GetClass()->ImplementsInterface(UInventoryInterface::StaticClass()))
	{
		return IInventoryInterface::Execute_GetInventoryManagerComponent(Actor);
	}

	// 3. Fallback direct component search
	return Actor->FindComponentByClass<UInventoryManagerComponent>();
}
