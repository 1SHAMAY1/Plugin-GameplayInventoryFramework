#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Fragment_ItemStats.generated.h"

/**
 * Fragment containing static base stats, weight, and tier for an item.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Item Stats & Weight Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_ItemStats : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_ItemStats();

	/** Base numeric stats mapped by Gameplay Tag (e.g. Item.Stat.Damage, Item.Stat.Armor) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (Categories = "Item.Stat"))
	TMap<FGameplayTag, float> BaseStats;

	/** Unit weight of this item in inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "0.0"))
	float Weight;

	/** Item quality / tier level (e.g. 1 = Common, 5 = Legendary) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta = (ClampMin = "1"))
	int32 ItemTier;

	/** Gets a stat value or fallback default if not present */
	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStatValue(UPARAM(meta = (Categories = "Item.Stat")) FGameplayTag StatTag, float DefaultValue = 0.0f) const;
};
