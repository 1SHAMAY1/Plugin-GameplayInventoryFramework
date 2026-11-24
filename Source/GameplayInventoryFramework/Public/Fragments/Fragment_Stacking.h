#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Fragment_Stacking.generated.h"

/**
 * Dedicated fragment defining item stacking, splitting, and combining rules.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Stacking Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Stacking : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Stacking();

	/** Maximum quantity that can occupy a single inventory slot */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stacking", meta = (ClampMin = "1"))
	int32 MaxStackCount = 99;

	/** Whether this item stack can be split into multiple smaller stacks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stacking")
	bool bCanSplit = true;

	/** Whether separate stacks of this item can be combined into one */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stacking")
	bool bCanCombine = true;
};
