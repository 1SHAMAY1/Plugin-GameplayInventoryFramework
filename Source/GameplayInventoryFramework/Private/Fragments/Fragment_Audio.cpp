#include "Fragments/Fragment_Audio.h"
#include "Types/InventoryNativeTags.h"

UFragment_Audio::UFragment_Audio()
{
	FragmentTag = MGITags::Item_Fragment_Audio;
}

const FItemAudioEntry* UFragment_Audio::FindAudioEntry(FGameplayTag AudioTag) const
{
	if (!AudioTag.IsValid())
	{
		return nullptr;
	}

	for (const FItemAudioEntry& Entry : AudioEntries)
	{
		if (Entry.AudioTag.MatchesTagExact(AudioTag) || Entry.AudioTag.MatchesTag(AudioTag))
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool UFragment_Audio::GetAudioEntry(FGameplayTag AudioTag, FItemAudioEntry& OutEntry) const
{
	if (const FItemAudioEntry* Found = FindAudioEntry(AudioTag))
	{
		OutEntry = *Found;
		return true;
	}
	return false;
}
