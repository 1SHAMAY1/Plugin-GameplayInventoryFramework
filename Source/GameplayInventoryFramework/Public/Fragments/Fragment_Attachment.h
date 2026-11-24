#pragma once

#include "CoreMinimal.h"
#include "Definitions/InventoryItemFragment.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Actor.h"
#include "Fragment_Attachment.generated.h"

class USceneComponent;

/**
 * Pure data fragment declaring visual attachment rules for an item.
 * Relies on Unreal's native USceneComponent::AttachToComponent without component bloat.
 * Features dedicated boolean toggles (bSpawnAsActor, bIsSkeletalMesh) with EditConditions.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Attachment Fragment"))
class GAMEPLAYINVENTORYFRAMEWORK_API UFragment_Attachment : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UFragment_Attachment();

	/** Target socket on character skeleton / weapon rail */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FName SocketName;

	/** Toggle between Actor spawning (true) and direct Component attachment (false) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	bool bSpawnAsActor;

	/** If attaching a component, toggles between Skeletal Mesh Component (true) and Static Mesh Component (false) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "!bSpawnAsActor"))
	bool bIsSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "!bSpawnAsActor && bIsSkeletalMesh"))
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "!bSpawnAsActor && !bIsSkeletalMesh"))
	TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment", meta = (EditCondition = "bSpawnAsActor"))
	TSoftClassPtr<AActor> SpawnedActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attachment")
	FTransform RelativeTransform;

	/** Spawns and attaches the visual representation to the target scene component using native USceneComponent::AttachToComponent */
	UFUNCTION(BlueprintCallable, Category = "Attachment")
	USceneComponent* AttachVisualToComponent(USceneComponent* ParentComponent) const;

	/** Spawns and attaches the actor representation if bSpawnAsActor is true */
	UFUNCTION(BlueprintCallable, Category = "Attachment")
	AActor* SpawnAndAttachActor(USceneComponent* ParentComponent) const;
};
