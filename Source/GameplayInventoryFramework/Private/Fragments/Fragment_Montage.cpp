#include "Fragments/Fragment_Montage.h"

UFragment_Montage::UFragment_Montage()
{
	FragmentTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Fragment.Montage"), false);
}

const FItemMontageEntry* UFragment_Montage::FindMontageEntry(FGameplayTag InMontageTag) const
{
	if (!InMontageTag.IsValid())
	{
		return nullptr;
	}

	for (const FItemMontageEntry& Entry : Montages)
	{
		if (Entry.MontageTag.MatchesTagExact(InMontageTag) || Entry.MontageTag.MatchesTag(InMontageTag))
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool UFragment_Montage::GetMontageEntry(FGameplayTag InMontageTag, FItemMontageEntry& OutEntry) const
{
	if (const FItemMontageEntry* Found = FindMontageEntry(InMontageTag))
	{
		OutEntry = *Found;
		return true;
	}
	return false;
}
