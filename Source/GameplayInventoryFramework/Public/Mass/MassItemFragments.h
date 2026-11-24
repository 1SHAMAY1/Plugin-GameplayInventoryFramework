#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "GameplayTagContainer.h"
#include "MassItemFragments.generated.h"

/**
 * ECS Mass Fragment carrying data for lightweight high-volume ground loot drops.
 */
USTRUCT()
struct GAMEPLAYINVENTORYFRAMEWORK_API FMassItemDropFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "MassItem")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, Category = "MassItem")
	int32 Quantity = 1;
};

/**
 * ECS Mass Marker Tag (zero memory footprint) identifying item drop entities.
 */
USTRUCT()
struct GAMEPLAYINVENTORYFRAMEWORK_API FMassItemDropTag : public FMassTag
{
	GENERATED_BODY()
};
