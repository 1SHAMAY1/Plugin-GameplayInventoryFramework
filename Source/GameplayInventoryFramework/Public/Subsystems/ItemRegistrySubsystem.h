#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Engine/StreamableManager.h"
#include "ItemRegistrySubsystem.generated.h"

class UInventoryItemDefinition;

/**
 * GameInstance subsystem providing global item definition cataloging and asynchronous memory management.
 * Backend streaming and registration are pure C++; catalog queries are exposed to Blueprint.
 */
UCLASS()
class GAMEPLAYINVENTORYFRAMEWORK_API UItemRegistrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- Backend Catalog Registration (Pure C++ - initialized during startup/asset discovery) ---
	void RegisterItemDefinition(const UInventoryItemDefinition* ItemDef);

	// --- Designer & Gameplay Lookup (Blueprint Facing) ---
	UFUNCTION(BlueprintPure, Category = "ItemRegistry", meta = (Categories = "Item.Type"))
	const UInventoryItemDefinition* GetItemDefinition(UPARAM(meta = (Categories = "Item.Type")) FGameplayTag ItemTag) const;

	// --- Memory-Conscious Streamable Handle Management (Pure C++ Engine Level) ---
	void RequestAsyncLoadItemAssets(const UInventoryItemDefinition* ItemDef, FStreamableDelegate OnLoadedCallback, TAsyncLoadPriority Priority = 0);
	void ReleaseItemAssets(const UInventoryItemDefinition* ItemDef);
	bool HasActiveStreamableHandle(const UInventoryItemDefinition* ItemDef) const;

protected:
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<const UInventoryItemDefinition>> ItemDefinitionCatalog;

	TMap<FGameplayTag, TSharedPtr<FStreamableHandle>> ActiveStreamableHandles;
	FStreamableManager StreamableManager;
};
