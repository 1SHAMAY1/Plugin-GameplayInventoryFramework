#include "Fragments/Fragment_ItemStats.h"

UFragment_ItemStats::UFragment_ItemStats()
	: Weight(0.0f)
	, ItemTier(1)
{
	FragmentTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Fragment.Stats"), false);
}

float UFragment_ItemStats::GetStatValue(FGameplayTag StatTag, float DefaultValue) const
{
	if (const float* Found = BaseStats.Find(StatTag))
	{
		return *Found;
	}
	return DefaultValue;
}
