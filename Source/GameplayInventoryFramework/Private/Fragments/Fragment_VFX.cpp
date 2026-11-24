#include "Fragments/Fragment_VFX.h"
#include "Types/InventoryNativeTags.h"

UFragment_VFX::UFragment_VFX()
{
	FragmentTag = MGITags::Item_Fragment_VFX;
}

const FItemVFXEntry* UFragment_VFX::FindVFXEntry(FGameplayTag VFXTag) const
{
	if (!VFXTag.IsValid())
	{
		return nullptr;
	}

	for (const FItemVFXEntry& Entry : VFXEntries)
	{
		if (Entry.VFXTag.MatchesTagExact(VFXTag) || Entry.VFXTag.MatchesTag(VFXTag))
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool UFragment_VFX::GetVFXEntry(FGameplayTag VFXTag, FItemVFXEntry& OutEntry) const
{
	if (const FItemVFXEntry* Found = FindVFXEntry(VFXTag))
	{
		OutEntry = *Found;
		return true;
	}
	return false;
}
