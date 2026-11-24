#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Sound/SoundBase.h"
#include "Fragment_Audio.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemAudioEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (Categories = "Audio.Item"))
	FGameplayTag AudioTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	FTransform RelativeTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (ClampMin = "0.0"))
	float VolumeMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio", meta = (ClampMin = "0.01"))
	float PitchMultiplier = 1.0f;
};

/**
 * Dedicated fragment for audio cues and MetaSounds.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Audio Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Audio : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Audio();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TArray<FItemAudioEntry> AudioEntries;

	/** Finds an audio entry by tag (C++ helper) */
	const FItemAudioEntry* FindAudioEntry(FGameplayTag AudioTag) const;

	/** Retrieves an audio entry by tag (Blueprint helper) */
	UFUNCTION(BlueprintPure, Category = "Audio", meta = (Categories = "Audio.Item"))
	bool GetAudioEntry(UPARAM(meta = (Categories = "Audio.Item")) FGameplayTag AudioTag, FItemAudioEntry& OutEntry) const;
};
