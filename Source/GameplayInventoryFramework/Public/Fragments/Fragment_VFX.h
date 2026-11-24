#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Particles/ParticleSystem.h"
#include "NiagaraSystem.h"
#include "Fragment_VFX.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemVFXEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX", meta = (Categories = "VFX.Item"))
	FGameplayTag VFXTag;

	/** Toggle between Niagara System and legacy Cascade Particle System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	bool bIsNiagara = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX", meta = (EditCondition = "bIsNiagara"))
	TSoftObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX", meta = (EditCondition = "!bIsNiagara"))
	TSoftObjectPtr<UParticleSystem> CascadeSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	FTransform RelativeTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	bool bAutoActivate = true;
};

/**
 * Dedicated fragment for Niagara and particle visual effects.
 */
UCLASS(BlueprintType, meta = (DisplayName = "VFX Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_VFX : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_VFX();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	TArray<FItemVFXEntry> VFXEntries;

	/** Finds a VFX entry by tag (C++ helper) */
	const FItemVFXEntry* FindVFXEntry(FGameplayTag VFXTag) const;

	/** Retrieves a VFX entry by tag (Blueprint helper) */
	UFUNCTION(BlueprintPure, Category = "VFX", meta = (Categories = "VFX.Item"))
	bool GetVFXEntry(UPARAM(meta = (Categories = "VFX.Item")) FGameplayTag VFXTag, FItemVFXEntry& OutEntry) const;
};
