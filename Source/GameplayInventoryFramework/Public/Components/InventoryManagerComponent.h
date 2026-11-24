#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventoryManagerComponent.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UInventoryItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnItemTransferred, UInventoryComponent*, SourceInv, UInventoryComponent*, TargetInv, UInventoryItemInstance*, Item, int32, Count);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnItemTransferredNative, UInventoryComponent*, UInventoryComponent*, UInventoryItemInstance*, int32);

/**
 * Multi-inventory orchestrator.
 * Coordinates multi-container registration and atomic cross-container transactions.
 * Internal framework plumbing remains pure C++; only gameplay operations are exposed to Blueprint.
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryManagerComponent();
	virtual void BeginPlay() override;

	// --- Backend Registration (Pure C++ - internal lifecycle plumbing) ---
	void RegisterInventory(UInventoryComponent* InventoryComp);
	void UnregisterInventory(UInventoryComponent* InventoryComp);
	const TArray<TWeakObjectPtr<UInventoryComponent>>& GetRegisteredInventories() const { return RegisteredInventories; }

	// --- Designer & Gameplay Accessors ---
	UFUNCTION(BlueprintPure, Category = "InventoryManager|Containers", meta = (Categories = "Inventory.Type"))
	UInventoryComponent* FindInventoryByTag(UPARAM(meta = (Categories = "Inventory.Type")) FGameplayTag ContainerTag) const;

	// --- Atomic Cross-Inventory Operations (Designer & UI Facing) ---
	UFUNCTION(BlueprintCallable, Category = "InventoryManager|Operations")
	bool TransferItem(UInventoryComponent* SourceInv, UInventoryComponent* TargetInv, UInventoryItemInstance* ItemInstance, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager|Operations")
	bool SwapSlots(UInventoryComponent* SourceInv, int32 SourceIndex, UInventoryComponent* TargetInv, int32 TargetIndex);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager|Operations")
	bool SplitStack(UInventoryComponent* SourceInv, int32 SourceIndex, int32 SplitCount, UInventoryComponent* TargetInv);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager|Operations")
	int32 LootAll(UInventoryComponent* SourceInv, UInventoryComponent* TargetInv);

	UFUNCTION(BlueprintCallable, Category = "InventoryManager|Operations")
	bool AutoRouteItem(UInventoryItemInstance* Instance, int32 Count = 1);

	// --- Delegates ---
	UPROPERTY(BlueprintAssignable, Category = "InventoryManager|Delegates")
	FOnItemTransferred OnItemTransferred;

	FOnItemTransferredNative OnItemTransferredNative;

protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UInventoryComponent>> RegisteredInventories;
};
