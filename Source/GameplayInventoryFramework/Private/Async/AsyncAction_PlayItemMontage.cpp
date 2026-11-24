#include "Async/AsyncAction_PlayItemMontage.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Instances/InventoryItemInstance.h"
#include "Fragments/Fragment_Montage.h"

UAsyncAction_PlayItemMontage* UAsyncAction_PlayItemMontage::PlayItemMontageAsync(
	UObject* WorldContextObject,
	ACharacter* TargetCharacter,
	const UInventoryItemInstance* ItemInstance,
	FGameplayTag MontageTag,
	float PlayRateOverride)
{
	UAsyncAction_PlayItemMontage* Action = NewObject<UAsyncAction_PlayItemMontage>();
	Action->RegisterWithGameInstance(WorldContextObject);
	Action->CachedCharacter = TargetCharacter;
	Action->CachedItem = ItemInstance;
	Action->CachedMontageTag = MontageTag;
	Action->CachedPlayRate = PlayRateOverride;
	return Action;
}

void UAsyncAction_PlayItemMontage::Activate()
{
	if (!CachedCharacter.IsValid() || !CachedItem.IsValid() || !CachedMontageTag.IsValid())
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	ACharacter* Character = CachedCharacter.Get();
	USkeletalMeshComponent* Mesh = Character ? Character->GetMesh() : nullptr;
	UAnimInstance* AnimInstance = Mesh ? Mesh->GetAnimInstance() : nullptr;

	if (!AnimInstance)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	const UFragment_Montage* MontageFrag = CachedItem->FindFragmentByClass<UFragment_Montage>();
	if (!MontageFrag)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	const FItemMontageEntry* Entry = MontageFrag->FindMontageEntry(CachedMontageTag);
	if (!Entry || Entry->AnimMontage.IsNull())
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	UAnimMontage* MontageToPlay = Entry->AnimMontage.LoadSynchronous();
	if (!MontageToPlay)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	BlendingOutDelegate.BindUObject(this, &UAsyncAction_PlayItemMontage::HandleMontageBlendingOut);
	AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

	FOnMontageEnded EndedDelegate;
	EndedDelegate.BindUObject(this, &UAsyncAction_PlayItemMontage::HandleMontageEnded);
	AnimInstance->Montage_SetEndDelegate(EndedDelegate, MontageToPlay);

	const float Rate = (CachedPlayRate > 0.0f) ? CachedPlayRate : Entry->PlayRate;
	const float Duration = AnimInstance->Montage_Play(MontageToPlay, Rate);

	if (Duration <= 0.0f)
	{
		OnFailed.Broadcast();
		SetReadyToDestroy();
		return;
	}

	PlayingMontage = MontageToPlay;
	CachedAnimInstance = AnimInstance;
	bPlayed = true;

	if (!Entry->StartingSection.IsNone())
	{
		AnimInstance->Montage_JumpToSection(Entry->StartingSection, MontageToPlay);
	}
}

void UAsyncAction_PlayItemMontage::HandleMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == PlayingMontage.Get())
	{
		OnBlendOut.Broadcast();
	}
}

void UAsyncAction_PlayItemMontage::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == PlayingMontage.Get())
	{
		if (bInterrupted)
		{
			OnInterrupted.Broadcast();
		}
		else
		{
			OnCompleted.Broadcast();
		}
		SetReadyToDestroy();
	}
}
