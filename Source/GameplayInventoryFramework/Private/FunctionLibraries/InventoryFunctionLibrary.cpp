#include "FunctionLibraries/InventoryFunctionLibrary.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Fragments/Fragment_Attachment.h"
#include "Fragments/Fragment_Montage.h"
#include "Subsystems/ItemRegistrySubsystem.h"
#include "Subsystems/WorldItemSubsystem.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"

USceneComponent* UInventoryFunctionLibrary::AttachItemVisual(USceneComponent* TargetMesh, const UInventoryItemInstance* ItemInstance)
{
	if (!TargetMesh || !ItemInstance)
	{
		return nullptr;
	}

	const UFragment_Attachment* AttachFrag = ItemInstance->FindFragmentByClass<UFragment_Attachment>();
	if (!AttachFrag)
	{
		return nullptr;
	}

	return AttachFrag->AttachVisualToComponent(TargetMesh);
}

void UInventoryFunctionLibrary::DetachItemVisual(USceneComponent* TargetMesh, FName SocketName)
{
	if (!TargetMesh)
	{
		return;
	}

	TArray<USceneComponent*> Children = TargetMesh->GetAttachChildren();
	for (USceneComponent* Child : Children)
	{
		if (Child && (SocketName.IsNone() || Child->GetAttachSocketName() == SocketName))
		{
			AActor* ChildOwner = Child->GetOwner();
			if (ChildOwner && ChildOwner != TargetMesh->GetOwner())
			{
				ChildOwner->Destroy();
			}
			else
			{
				Child->DestroyComponent();
			}
		}
	}
}

const UInventoryItemFragment* UInventoryFunctionLibrary::GetFragmentByTag(const UInventoryItemDefinition* ItemDef, FGameplayTag FragmentTag)
{
	return ItemDef ? ItemDef->FindFragmentByTag(FragmentTag) : nullptr;
}

bool UInventoryFunctionLibrary::HasFragmentTag(const UInventoryItemDefinition* ItemDef, FGameplayTag FragmentTag)
{
	return ItemDef && (ItemDef->FindFragmentByTag(FragmentTag) != nullptr);
}

bool UInventoryFunctionLibrary::GetMontageByTag(const UInventoryItemDefinition* ItemDef, FGameplayTag MontageTag, FItemMontageEntry& OutEntry)
{
	if (!ItemDef)
	{
		return false;
	}

	const UFragment_Montage* MontageFrag = ItemDef->FindFragmentByClass<UFragment_Montage>();
	return MontageFrag ? MontageFrag->GetMontageEntry(MontageTag, OutEntry) : false;
}

float UInventoryFunctionLibrary::GetItemDynamicFloat(const UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, float DefaultValue)
{
	return ItemInstance ? ItemInstance->GetParamFloat(StatTag, DefaultValue) : DefaultValue;
}

void UInventoryFunctionLibrary::SetItemDynamicFloat(UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, float Value)
{
	if (ItemInstance)
	{
		ItemInstance->SetParamFloat(StatTag, Value);
	}
}

int32 UInventoryFunctionLibrary::GetItemDynamicInt(const UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, int32 DefaultValue)
{
	return ItemInstance ? ItemInstance->GetParamInt(StatTag, DefaultValue) : DefaultValue;
}

void UInventoryFunctionLibrary::SetItemDynamicInt(UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, int32 Value)
{
	if (ItemInstance)
	{
		ItemInstance->SetParamInt(StatTag, Value);
	}
}

bool UInventoryFunctionLibrary::GetItemDynamicBool(const UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, bool DefaultValue)
{
	return ItemInstance ? ItemInstance->GetParamBool(StatTag, DefaultValue) : DefaultValue;
}

void UInventoryFunctionLibrary::SetItemDynamicBool(UInventoryItemInstance* ItemInstance, FGameplayTag StatTag, bool Value)
{
	if (ItemInstance)
	{
		ItemInstance->SetParamBool(StatTag, Value);
	}
}

UItemRegistrySubsystem* UInventoryFunctionLibrary::GetItemRegistrySubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	if (World && World->GetGameInstance())
	{
		return World->GetGameInstance()->GetSubsystem<UItemRegistrySubsystem>();
	}
	return nullptr;
}

UWorldItemSubsystem* UInventoryFunctionLibrary::GetWorldItemSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<UWorldItemSubsystem>() : nullptr;
}
