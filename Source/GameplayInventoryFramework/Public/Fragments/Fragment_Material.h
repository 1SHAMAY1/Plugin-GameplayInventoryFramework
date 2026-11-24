#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Materials/MaterialInterface.h"
#include "Fragment_Material.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemMaterialSlotOverride
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	int32 MaterialIndex = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	TSoftObjectPtr<UMaterialInterface> Material;
};

/**
 * Dedicated fragment for materials and material overrides.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Material Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Material : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Material();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Material")
	TArray<FItemMaterialSlotOverride> MaterialOverrides;
};
