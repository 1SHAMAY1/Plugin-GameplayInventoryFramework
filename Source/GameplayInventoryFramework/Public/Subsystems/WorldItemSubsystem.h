#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "World/ItemComponent.h"
#include "WorldItemSubsystem.generated.h"

class UItemComponent;

/**
 * World subsystem indexing world ground items for spatial AOE loot queries.
 * Actor registration is pure C++; spatial queries are exposed to Blueprint.
 */
UCLASS()
class GAMEPLAYINVENTORYFRAMEWORK_API UWorldItemSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	// --- Backend Lifecycle Hooks (Pure C++ - called by UItemComponent BeginPlay/EndPlay) ---
	void RegisterWorldItem(UItemComponent* ItemComp);
	void UnregisterWorldItem(UItemComponent* ItemComp);
	int32 GetRegisteredItemCount() const;

	// --- Designer & Gameplay Queries (Blueprint Facing) ---
	UFUNCTION(BlueprintCallable, Category = "WorldItems")
	TArray<UItemComponent*> FindNearbyItems(FVector Center, float Radius, const FGameplayTagQuery& FilterQuery = FGameplayTagQuery()) const;

protected:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UItemComponent>> WorldItems;
};
