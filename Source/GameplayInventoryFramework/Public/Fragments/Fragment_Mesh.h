#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Fragment_Mesh.generated.h"

/**
 * Dedicated mesh fragment supporting both static and skeletal meshes with detection.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Mesh Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Mesh : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Mesh();

	/** If true, uses the SkeletalMesh reference; otherwise uses the StaticMesh reference */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	bool bIsSkeletal = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (EditCondition = "bIsSkeletal"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh", meta = (EditCondition = "!bIsSkeletal"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	FTransform RelativeTransform = FTransform::Identity;
};
