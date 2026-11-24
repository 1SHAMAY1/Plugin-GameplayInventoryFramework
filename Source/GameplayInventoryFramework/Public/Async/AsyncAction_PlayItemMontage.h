#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_PlayItemMontage.generated.h"

class ACharacter;
class UInventoryItemInstance;
class UAnimMontage;
class UAnimInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMontagePlayDelegate);

/**
 * Designer-friendly Blueprint async action for playing item-tagged animations.
 * Provides OnCompleted, OnBlendOut, OnInterrupted, and OnFailed execution pins.
 */
UCLASS()
class GAMEPLAYINVENTORYFRAMEWORK_API UAsyncAction_PlayItemMontage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FOnMontagePlayDelegate OnFailed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Categories = "Animation.Montage"), Category = "Inventory|Animation")
	static UAsyncAction_PlayItemMontage* PlayItemMontageAsync(
		UObject* WorldContextObject,
		ACharacter* TargetCharacter,
		const UInventoryItemInstance* ItemInstance,
		UPARAM(meta = (Categories = "Animation.Montage")) FGameplayTag MontageTag,
		float PlayRateOverride = 0.0f
	);

	virtual void Activate() override;

private:
	void HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	TWeakObjectPtr<ACharacter> CachedCharacter;
	TWeakObjectPtr<const UInventoryItemInstance> CachedItem;
	FGameplayTag CachedMontageTag;
	float CachedPlayRate = 0.0f;
	TWeakObjectPtr<UAnimInstance> CachedAnimInstance;
	TWeakObjectPtr<UAnimMontage> PlayingMontage;
	bool bPlayed = false;
};
