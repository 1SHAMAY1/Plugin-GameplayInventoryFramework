#include "Subsystems/ItemRegistrySubsystem.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_Mesh.h"
#include "Fragments/Fragment_Texture.h"
#include "Fragments/Fragment_Montage.h"
#include "Fragments/Fragment_Audio.h"
#include "Fragments/Fragment_VFX.h"

void UItemRegistrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ItemDefinitionCatalog.Empty();
	ActiveStreamableHandles.Empty();
}

void UItemRegistrySubsystem::Deinitialize()
{
	for (auto& Pair : ActiveStreamableHandles)
	{
		if (Pair.Value.IsValid())
		{
			Pair.Value->CancelHandle();
		}
	}
	ActiveStreamableHandles.Empty();
	ItemDefinitionCatalog.Empty();

	Super::Deinitialize();
}

void UItemRegistrySubsystem::RegisterItemDefinition(const UInventoryItemDefinition* ItemDef)
{
	if (ItemDef && ItemDef->ItemIdentifierTag.IsValid())
	{
		ItemDefinitionCatalog.Add(ItemDef->ItemIdentifierTag, ItemDef);
	}
}

const UInventoryItemDefinition* UItemRegistrySubsystem::GetItemDefinition(FGameplayTag ItemTag) const
{
	if (!ItemTag.IsValid())
	{
		return nullptr;
	}

	if (const TObjectPtr<const UInventoryItemDefinition>* Found = ItemDefinitionCatalog.Find(ItemTag))
	{
		return Found->Get();
	}

	return nullptr;
}

void UItemRegistrySubsystem::RequestAsyncLoadItemAssets(const UInventoryItemDefinition* ItemDef, FStreamableDelegate OnLoadedCallback, TAsyncLoadPriority Priority)
{
	if (!ItemDef)
	{
		OnLoadedCallback.ExecuteIfBound();
		return;
	}

	TArray<FSoftObjectPath> TargetsToStream;

	for (const TObjectPtr<UInventoryItemFragment>& Fragment : ItemDef->Fragments)
	{
		if (!Fragment)
		{
			continue;
		}

		if (const UFragment_Mesh* MeshFrag = Cast<UFragment_Mesh>(Fragment))
		{
			if (!MeshFrag->StaticMesh.IsNull())
			{
				TargetsToStream.Add(MeshFrag->StaticMesh.ToSoftObjectPath());
			}
			if (!MeshFrag->SkeletalMesh.IsNull())
			{
				TargetsToStream.Add(MeshFrag->SkeletalMesh.ToSoftObjectPath());
			}
		}
		else if (const UFragment_Texture* TexFrag = Cast<UFragment_Texture>(Fragment))
		{
			for (const FItemTextureEntry& TexEntry : TexFrag->Textures)
			{
				if (!TexEntry.Texture.IsNull())
				{
					TargetsToStream.Add(TexEntry.Texture.ToSoftObjectPath());
				}
			}
		}
		else if (const UFragment_Montage* MontageFrag = Cast<UFragment_Montage>(Fragment))
		{
			for (const FItemMontageEntry& Entry : MontageFrag->Montages)
			{
				if (!Entry.AnimMontage.IsNull())
				{
					TargetsToStream.Add(Entry.AnimMontage.ToSoftObjectPath());
				}
			}
		}
		else if (const UFragment_Audio* AudioFrag = Cast<UFragment_Audio>(Fragment))
		{
			for (const FItemAudioEntry& Entry : AudioFrag->AudioEntries)
			{
				if (!Entry.Sound.IsNull())
				{
					TargetsToStream.Add(Entry.Sound.ToSoftObjectPath());
				}
			}
		}
		else if (const UFragment_VFX* VFXFrag = Cast<UFragment_VFX>(Fragment))
		{
			for (const FItemVFXEntry& Entry : VFXFrag->VFXEntries)
			{
				if (!Entry.NiagaraSystem.IsNull())
				{
					TargetsToStream.Add(Entry.NiagaraSystem.ToSoftObjectPath());
				}
				if (!Entry.CascadeSystem.IsNull())
				{
					TargetsToStream.Add(Entry.CascadeSystem.ToSoftObjectPath());
				}
			}
		}
	}

	if (TargetsToStream.Num() > 0)
	{
		TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(TargetsToStream, OnLoadedCallback, Priority);
		if (ItemDef->ItemIdentifierTag.IsValid())
		{
			ActiveStreamableHandles.Add(ItemDef->ItemIdentifierTag, Handle);
		}
	}
	else
	{
		OnLoadedCallback.ExecuteIfBound();
	}
}

void UItemRegistrySubsystem::ReleaseItemAssets(const UInventoryItemDefinition* ItemDef)
{
	if (!ItemDef || !ItemDef->ItemIdentifierTag.IsValid())
	{
		return;
	}

	if (TSharedPtr<FStreamableHandle>* HandlePtr = ActiveStreamableHandles.Find(ItemDef->ItemIdentifierTag))
	{
		if (HandlePtr->IsValid())
		{
			(*HandlePtr)->CancelHandle();
		}
		ActiveStreamableHandles.Remove(ItemDef->ItemIdentifierTag);
	}
}

bool UItemRegistrySubsystem::HasActiveStreamableHandle(const UInventoryItemDefinition* ItemDef) const
{
	if (!ItemDef || !ItemDef->ItemIdentifierTag.IsValid())
	{
		return false;
	}

	return ActiveStreamableHandles.Contains(ItemDef->ItemIdentifierTag);
}
