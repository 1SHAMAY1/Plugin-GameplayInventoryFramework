#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Animation/AnimMontage.h"
#include "Fragment_Montage.generated.h"

/**
 * Designer-friendly montage entry structure mapping a gameplay action tag to an animation.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemMontageEntry
{
	GENERATED_BODY()

	FItemMontageEntry()
		: PlayRate(1.0f)
		, StartingSection(NAME_None)
		, BlendInTime(0.25f)
		, BlendOutTime(0.25f)
	{
	}

	/** Action tag identifying this animation (e.g. Animation.Montage.Attack, Animation.Montage.Reload, Animation.Montage.Equip) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (Categories = "Animation.Montage"))
	FGameplayTag MontageTag;

	/** Soft reference to the Anim Montage (loaded on-demand to prevent memory spikes in MMORPGs) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TSoftObjectPtr<UAnimMontage> AnimMontage;

	/** Playback speed multiplier */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.01"))
	float PlayRate;

	/** Optional starting section name inside the montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	FName StartingSection;

	/** Blend in time when playing this montage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float BlendInTime;

	/** Blend out time when finishing or blending out */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	float BlendOutTime;
};

/**
 * Fragment containing multi-montage support for items (Equip, Unequip, Attack, Reload, Use, Inspect).
 */
UCLASS(BlueprintType, meta = (DisplayName = "Animation Montages Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Montage : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Montage();

	/** Array of designer-configured montages for various item actions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<FItemMontageEntry> Montages;

	/** Finds a montage entry matching the given tag */
	const FItemMontageEntry* FindMontageEntry(UPARAM(meta = (Categories = "Animation.Montage")) FGameplayTag InMontageTag) const;

	/** Blueprint accessor to find a montage entry */
	UFUNCTION(BlueprintPure, Category = "Animation")
	bool GetMontageEntry(UPARAM(meta = (Categories = "Animation.Montage")) FGameplayTag InMontageTag, FItemMontageEntry& OutEntry) const;
};
