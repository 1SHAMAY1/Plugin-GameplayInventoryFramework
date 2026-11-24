#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "InventoryItemFragment.generated.h"

class UInventoryItemInstance;

/**
 * Base polymorphic fragment representing modular item behavior or metadata.
 * Can be added to UInventoryItemDefinition assets in the editor.
 */
UCLASS(Abstract, DefaultToInstanced, EditInlineNew, BlueprintType, Blueprintable)
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemFragment();

	/** Primary identifying gameplay tag for this fragment (e.g. Item.Fragment.UI, Item.Fragment.Montage) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragment", meta = (Categories = "Item.Fragment"))
	FGameplayTag FragmentTag;

	/** Additional gameplay tags describing features or restrictions of this fragment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragment")
	FGameplayTagContainer AdditionalTags;

	/** Checks if this fragment matches the given tag exactly or as a child tag */
	UFUNCTION(BlueprintPure, Category = "Fragment")
	bool MatchesTag(FGameplayTag InTag) const;

	/** Checks if this fragment matches the given tag exactly */
	UFUNCTION(BlueprintPure, Category = "Fragment")
	bool MatchesTagExact(FGameplayTag InTag) const;

	/** Checks if this fragment satisfies the specified tag query */
	UFUNCTION(BlueprintPure, Category = "Fragment")
	bool HasMatchingTags(const FGameplayTagQuery& TagQuery) const;

	/** Called when an item instance is created from the owning definition */
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const {}
};
