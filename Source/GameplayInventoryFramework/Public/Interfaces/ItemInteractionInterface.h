#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInteractionInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UItemInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface implemented by interactive world items, dropped loot, and container props.
 */
class GAMEPLAYINVENTORYFRAMEWORK_API IItemInteractionInterface
{
	GENERATED_BODY()

public:
	/** Checks if the interactor is allowed to interact with this item */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Interaction")
	bool CanInteract(AActor* Interactor) const;

	/** Retrieves the interaction prompt text, item name, and stack count */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Interaction")
	void GetInteractionPrompt(FText& OutActionPrompt, FText& OutItemName, int32& OutCount) const;

	/** Executes the interaction (e.g. looting or opening) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item|Interaction")
	bool ExecuteInteract(AActor* Interactor);

	/** Seamlessly checks interaction viability without concrete class casting */
	static bool CanInteractWithActor(const AActor* TargetActor, AActor* Interactor);

	/** Seamlessly triggers interaction without concrete class casting */
	static bool InteractWithActor(AActor* TargetActor, AActor* Interactor);
};
