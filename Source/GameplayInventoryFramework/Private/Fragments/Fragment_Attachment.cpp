#include "Fragments/Fragment_Attachment.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Types/InventoryNativeTags.h"

UFragment_Attachment::UFragment_Attachment()
	: SocketName(NAME_None)
	, bSpawnAsActor(false)
	, bIsSkeletalMesh(false)
	, RelativeTransform(FTransform::Identity)
{
	FragmentTag = MGITags::Item_Fragment_Attachment;
}

USceneComponent* UFragment_Attachment::AttachVisualToComponent(USceneComponent* ParentComponent) const
{
	if (!ParentComponent)
	{
		return nullptr;
	}

	if (bSpawnAsActor)
	{
		AActor* Spawned = SpawnAndAttachActor(ParentComponent);
		return Spawned ? Spawned->GetRootComponent() : nullptr;
	}

	AActor* OwnerActor = ParentComponent->GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	if (bIsSkeletalMesh)
	{
		USkeletalMeshComponent* SkelComp = NewObject<USkeletalMeshComponent>(OwnerActor);
		if (USkeletalMesh* Mesh = SkeletalMesh.LoadSynchronous())
		{
			SkelComp->SetSkeletalMesh(Mesh);
		}
		SkelComp->AttachToComponent(ParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		SkelComp->SetRelativeTransform(RelativeTransform);
		SkelComp->RegisterComponent();
		return SkelComp;
	}
	else
	{
		UStaticMeshComponent* StaticComp = NewObject<UStaticMeshComponent>(OwnerActor);
		if (UStaticMesh* Mesh = StaticMesh.LoadSynchronous())
		{
			StaticComp->SetStaticMesh(Mesh);
		}
		StaticComp->AttachToComponent(ParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		StaticComp->SetRelativeTransform(RelativeTransform);
		StaticComp->RegisterComponent();
		return StaticComp;
	}
}

AActor* UFragment_Attachment::SpawnAndAttachActor(USceneComponent* ParentComponent) const
{
	if (!ParentComponent)
	{
		return nullptr;
	}

	UWorld* World = ParentComponent->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	UClass* LoadedClass = SpawnedActorClass.LoadSynchronous();
	if (!LoadedClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = ParentComponent->GetOwner();

	AActor* SpawnedActor = World->SpawnActor<AActor>(LoadedClass, SpawnParams);
	if (SpawnedActor)
	{
		if (!SpawnedActor->GetRootComponent())
		{
			USceneComponent* NewRoot = NewObject<USceneComponent>(SpawnedActor, TEXT("RootComponent"));
			SpawnedActor->SetRootComponent(NewRoot);
			NewRoot->RegisterComponent();
		}

		SpawnedActor->AttachToComponent(ParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		SpawnedActor->SetActorRelativeTransform(RelativeTransform);
	}

	return SpawnedActor;
}
