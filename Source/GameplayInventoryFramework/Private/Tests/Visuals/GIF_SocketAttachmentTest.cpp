// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Fragments/Fragment_Attachment.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFSocketAttachmentTest, "GameplayInventory.Visuals.SocketAttachment",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFSocketAttachmentTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	TestNotNull(TEXT("World must be valid"), World);
	if (!World)
	{
		return false;
	}

	// 1. Create Parent Character Actor and Scene Component
	AActor* ParentActor = World->SpawnActor<AActor>();
	USceneComponent* ParentComp = NewObject<USceneComponent>(ParentActor, TEXT("ParentMesh"));
	ParentActor->SetRootComponent(ParentComp);
	ParentComp->RegisterComponent();

	// ----------------------------------------------------
	// Test 1: Static Mesh Attachment Component
	// ----------------------------------------------------
	UFragment_Attachment* StaticAttachFrag = NewObject<UFragment_Attachment>();
	StaticAttachFrag->bSpawnAsActor = false;
	StaticAttachFrag->bIsSkeletalMesh = false;
	StaticAttachFrag->SocketName = FName(TEXT("WeaponSocket_R"));
	StaticAttachFrag->RelativeTransform = FTransform(FRotator(0.0f, 90.0f, 0.0f), FVector(10.0f, 20.0f, 30.0f), FVector(1.0f, 1.0f, 1.0f));

	USceneComponent* AttachedStaticComp = StaticAttachFrag->AttachVisualToComponent(ParentComp);
	TestNotNull(TEXT("AttachedStaticComp must be valid"), AttachedStaticComp);
	if (AttachedStaticComp)
	{
		TestTrue(TEXT("Attached component is UStaticMeshComponent"), AttachedStaticComp->IsA<UStaticMeshComponent>());
		TestEqual(TEXT("Attached component parent is ParentComp"), AttachedStaticComp->GetAttachParent(), ParentComp);
		TestEqual(TEXT("Attached component socket matches"), AttachedStaticComp->GetAttachSocketName(), FName(TEXT("WeaponSocket_R")));
		TestTrue(TEXT("Relative location matches"), AttachedStaticComp->GetRelativeLocation().Equals(FVector(10.0f, 20.0f, 30.0f), 0.1f));
	}

	// ----------------------------------------------------
	// Test 2: Skeletal Mesh Attachment Component
	// ----------------------------------------------------
	UFragment_Attachment* SkelAttachFrag = NewObject<UFragment_Attachment>();
	SkelAttachFrag->bSpawnAsActor = false;
	SkelAttachFrag->bIsSkeletalMesh = true;
	SkelAttachFrag->SocketName = FName(TEXT("BackpackSocket"));
	SkelAttachFrag->RelativeTransform = FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 50.0f), FVector(1.0f, 1.0f, 1.0f));

	USceneComponent* AttachedSkelComp = SkelAttachFrag->AttachVisualToComponent(ParentComp);
	TestNotNull(TEXT("AttachedSkelComp must be valid"), AttachedSkelComp);
	if (AttachedSkelComp)
	{
		TestTrue(TEXT("Attached component is USkeletalMeshComponent"), AttachedSkelComp->IsA<USkeletalMeshComponent>());
		TestEqual(TEXT("Attached component parent is ParentComp"), AttachedSkelComp->GetAttachParent(), ParentComp);
		TestEqual(TEXT("Attached component socket matches"), AttachedSkelComp->GetAttachSocketName(), FName(TEXT("BackpackSocket")));
	}

	// ----------------------------------------------------
	// Test 3: Spawned Actor Attachment
	// ----------------------------------------------------
	UFragment_Attachment* ActorAttachFrag = NewObject<UFragment_Attachment>();
	ActorAttachFrag->bSpawnAsActor = true;
	ActorAttachFrag->SpawnedActorClass = AActor::StaticClass();
	ActorAttachFrag->SocketName = FName(TEXT("HolsterSocket"));
	ActorAttachFrag->RelativeTransform = FTransform(FRotator(45.0f, 0.0f, 0.0f), FVector(5.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f));

	AActor* SpawnedActor = ActorAttachFrag->SpawnAndAttachActor(ParentComp);
	TestNotNull(TEXT("SpawnedActor must be valid"), SpawnedActor);
	if (SpawnedActor)
	{
		TestEqual(TEXT("Spawned actor attach parent is ParentActor"), SpawnedActor->GetAttachParentActor(), ParentActor);
		TestEqual(TEXT("Spawned actor socket matches"), SpawnedActor->GetRootComponent()->GetAttachSocketName(), FName(TEXT("HolsterSocket")));
	}

	// Cleanup
	World->DestroyWorld(false);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
