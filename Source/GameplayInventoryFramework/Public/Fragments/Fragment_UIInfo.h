#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Engine/Texture2D.h"
#include "Blueprint/UserWidget.h"
#include "Fragment_UIInfo.generated.h"

/**
 * Fragment containing presentation & UI display information for items.
 */
UCLASS(BlueprintType, meta = (DisplayName = "UI & Display Info Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_UIInfo : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_UIInfo();

	/** User-facing display name of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FText DisplayName;

	/** User-facing detailed description of the item */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (MultiLine = "true"))
	FText Description;

	/** Soft reference to the item's 2D icon texture (MMORPG memory optimization) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Optional soft reference to a specialized tooltip widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> CustomTooltipWidgetClass;
};
