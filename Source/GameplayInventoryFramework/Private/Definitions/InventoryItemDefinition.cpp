#include "Definitions/InventoryItemDefinition.h"
#include "Fragments/Fragment_Stacking.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UInventoryItemDefinition::UInventoryItemDefinition()
	: MaxStackCount(1)
{
}

int32 UInventoryItemDefinition::GetMaxStackCount() const
{
	if (const UFragment_Stacking* StackingFrag = FindFragmentByClass<UFragment_Stacking>())
	{
		return StackingFrag->MaxStackCount;
	}
	return MaxStackCount > 0 ? MaxStackCount : 1;
}

const UInventoryItemFragment* UInventoryItemDefinition::FindFragmentByTag(FGameplayTag Tag) const
{
	if (!Tag.IsValid())
	{
		return nullptr;
	}

	for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
	{
		if (Fragment && Fragment->MatchesTag(Tag))
		{
			return Fragment.Get();
		}
	}

	return nullptr;
}

TArray<UInventoryItemFragment*> UInventoryItemDefinition::FindFragmentsByTag(FGameplayTag Tag, bool bExactMatch) const
{
	TArray<UInventoryItemFragment*> Results;
	if (!Tag.IsValid())
	{
		return Results;
	}

	for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
	{
		if (Fragment)
		{
			const bool bMatches = bExactMatch ? Fragment->MatchesTagExact(Tag) : Fragment->MatchesTag(Tag);
			if (bMatches)
			{
				Results.Add(Fragment.Get());
			}
		}
	}

	return Results;
}

UInventoryItemFragment* UInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (!FragmentClass)
	{
		return nullptr;
	}

	for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			return Fragment.Get();
		}
	}

	return nullptr;
}

TArray<UInventoryItemFragment*> UInventoryItemDefinition::FindFragmentsByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	TArray<UInventoryItemFragment*> Results;
	if (!FragmentClass)
	{
		return Results;
	}

	for (const TObjectPtr<UInventoryItemFragment>& Fragment : Fragments)
	{
		if (Fragment && Fragment->IsA(FragmentClass))
		{
			Results.Add(Fragment.Get());
		}
	}

	return Results;
}

void UInventoryItemDefinition::AddFragment(UInventoryItemFragment* Fragment)
{
	if (Fragment)
	{
		Fragments.Add(Fragment);
	}
}

#if WITH_EDITOR
EDataValidationResult UInventoryItemDefinition::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (!ItemIdentifierTag.IsValid())
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("Item '%s' has an invalid or empty ItemIdentifierTag."), *GetName())));
		Result = EDataValidationResult::Invalid;
	}

	if (MaxStackCount < 1)
	{
		Context.AddError(FText::FromString(FString::Printf(TEXT("Item '%s' has MaxStackCount (%d) less than 1."), *GetName(), MaxStackCount)));
		Result = EDataValidationResult::Invalid;
	}

	// Validate fragments for null entries or duplicate tags
	TSet<FGameplayTag> EncounteredTags;
	for (int32 Index = 0; Index < Fragments.Num(); ++Index)
	{
		const UInventoryItemFragment* Fragment = Fragments[Index];
		if (!Fragment)
		{
			Context.AddWarning(FText::FromString(FString::Printf(TEXT("Item '%s' has a null fragment entry at index %d."), *GetName(), Index)));
			continue;
		}

		if (Fragment->FragmentTag.IsValid())
		{
			if (EncounteredTags.Contains(Fragment->FragmentTag))
			{
				Context.AddWarning(FText::FromString(FString::Printf(TEXT("Item '%s' has multiple fragments with identical tag '%s'."), *GetName(), *Fragment->FragmentTag.ToString())));
			}
			else
			{
				EncounteredTags.Add(Fragment->FragmentTag);
			}
		}
	}

	return Result;
}
#endif
