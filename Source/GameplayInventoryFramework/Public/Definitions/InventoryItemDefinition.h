#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Definitions/InventoryItemFragment.h"
#include "InventoryItemDefinition.generated.h"

/**
 * Static, data-driven definition of an item asset.
 * Inherits from UPrimaryDataAsset for asynchronous Asset Manager loading and CDO queries.
 */
UCLASS(BlueprintType, Blueprintable)
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition();

	/** Unique identification tag for this item type (e.g. Item.Type.Weapon.Sword, Item.Type.Consumable.Potion) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (Categories = "Item.Type"))
	FGameplayTag ItemIdentifierTag;

	/** Maximum allowed stack count for this item definition (1 for non-stackables like weapons/armor) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 MaxStackCount;

	/** Gets the max stack count from UFragment_Stacking if present, otherwise returns MaxStackCount (fallback 1) */
	UFUNCTION(BlueprintPure, Category = "Item")
	int32 GetMaxStackCount() const;

	/** Polymorphic fragments defining the item's static properties and behaviors */
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Item")
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

	/** Finds the first fragment matching the specified tag */
	UFUNCTION(BlueprintPure, Category = "Item")
	const UInventoryItemFragment* FindFragmentByTag(UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag Tag) const;

	/** Finds all fragments matching the specified tag (hierarchically by default, or exact) */
	UFUNCTION(BlueprintPure, Category = "Item", meta = (Categories = "Item.Fragment"))
	TArray<UInventoryItemFragment*> FindFragmentsByTag(UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag Tag, bool bExactMatch = false) const;

	/** Finds the first fragment of the specified class (Blueprint version) */
	UFUNCTION(BlueprintPure, Category = "Item", meta = (DeterminesOutputType = "FragmentClass"))
	UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	/** Finds all fragments of the specified class */
	UFUNCTION(BlueprintPure, Category = "Item")
	TArray<UInventoryItemFragment*> FindFragmentsByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	/** Finds the first fragment of template type T (C++ helper) */
	template<typename T>
	const T* FindFragmentByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UInventoryItemFragment>::Value, "'T' template parameter to FindFragmentByClass must be derived from UInventoryItemFragment");

		for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(T::StaticClass()))
			{
				return Cast<T>(Fragment.Get());
			}
		}
		return nullptr;
	}

	/** Finds all fragments of template type T (C++ helper) */
	template<typename T>
	TArray<const T*> FindFragmentsByClass() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, const UInventoryItemFragment>::Value, "'T' template parameter to FindFragmentsByClass must be derived from UInventoryItemFragment");

		TArray<const T*> Results;
		for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(T::StaticClass()))
			{
				Results.Add(Cast<T>(Fragment.Get()));
			}
		}
		return Results;
	}

	/** Adds a fragment programmatically (C++ helper for tests and procedural definitions) */
	void AddFragment(UInventoryItemFragment* Fragment);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
