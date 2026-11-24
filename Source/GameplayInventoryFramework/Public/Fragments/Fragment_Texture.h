#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Engine/Texture2D.h"
#include "Fragment_Texture.generated.h"

USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemTextureEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture", meta = (Categories = "Texture.Role"))
	FGameplayTag TextureRoleTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	TSoftObjectPtr<UTexture2D> Texture;
};

/**
 * Dedicated fragment for textures (Icons, Normal, ORM, Masks).
 */
UCLASS(BlueprintType, meta = (DisplayName = "Texture Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Texture : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Texture();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	TArray<FItemTextureEntry> Textures;

	UFUNCTION(BlueprintPure, Category = "Texture", meta = (Categories = "Texture.Role"))
	TSoftObjectPtr<UTexture2D> FindTextureByRole(UPARAM(meta = (Categories = "Texture.Role")) FGameplayTag RoleTag) const;
};
