#include "Definitions/InventoryItemFragment.h"

UInventoryItemFragment::UInventoryItemFragment()
{
}

bool UInventoryItemFragment::MatchesTag(FGameplayTag InTag) const
{
	if (!InTag.IsValid())
	{
		return false;
	}

	if (FragmentTag.MatchesTag(InTag))
	{
		return true;
	}

	return AdditionalTags.HasTag(InTag);
}

bool UInventoryItemFragment::MatchesTagExact(FGameplayTag InTag) const
{
	if (!InTag.IsValid())
	{
		return false;
	}

	if (FragmentTag.MatchesTagExact(InTag))
	{
		return true;
	}

	return AdditionalTags.HasTagExact(InTag);
}

bool UInventoryItemFragment::HasMatchingTags(const FGameplayTagQuery& TagQuery) const
{
	if (TagQuery.IsEmpty())
	{
		return true;
	}

	FGameplayTagContainer CombinedTags = AdditionalTags;
	if (FragmentTag.IsValid())
	{
		CombinedTags.AddTag(FragmentTag);
	}

	return TagQuery.Matches(CombinedTags);
}
