#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Components/InventoryManagerComponent.h"
#include "InventoryInterface.generated.h"

class UInventoryComponent;
class UInventoryManagerComponent;

UINTERFACE(MinimalAPI, BlueprintType)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that own or expose inventory containers (Characters, Chests, Vehicles, Corpses, Vendors).
 * Allows UI and gameplay systems to retrieve inventories with zero concrete class casting.
 */
class GAMEPLAYINVENTORYFRAMEWORK_API IInventoryInterface
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves an inventory component matching the specified container type tag.
	 * If ContainerTag is empty/invalid, returns the primary/default inventory component.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory", meta = (Categories = "Inventory.Type"))
	UInventoryComponent* GetInventoryComponent(UPARAM(meta = (Categories = "Inventory.Type")) FGameplayTag ContainerTag = FGameplayTag()) const;

	/**
	 * Retrieves the inventory manager component (if this actor manages multiple inventories).
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	UInventoryManagerComponent* GetInventoryManagerComponent() const;

	/**
	 * Seamlessly resolves the inventory component from any Actor without concrete class casting.
	 * Checks C++ native interface first, then Blueprint interface, and falls back to component search.
	 */
	static UInventoryComponent* ResolveInventory(const AActor* Actor, FGameplayTag ContainerTag = FGameplayTag());

	/**
	 * Seamlessly resolves the inventory manager component from any Actor without concrete class casting.
	 */
	static UInventoryManagerComponent* ResolveInventoryManager(const AActor* Actor);
};
