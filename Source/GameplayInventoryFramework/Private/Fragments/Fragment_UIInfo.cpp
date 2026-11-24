#include "Fragments/Fragment_UIInfo.h"

UFragment_UIInfo::UFragment_UIInfo()
{
	FragmentTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Fragment.UI"), false);
}
