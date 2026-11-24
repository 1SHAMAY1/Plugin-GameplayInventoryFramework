#include "Fragments/Fragment_Texture.h"
#include "Types/InventoryNativeTags.h"

UFragment_Texture::UFragment_Texture()
{
	FragmentTag = MGITags::Item_Fragment_Texture;
}

TSoftObjectPtr<UTexture2D> UFragment_Texture::FindTextureByRole(FGameplayTag RoleTag) const
{
	if (!RoleTag.IsValid())
	{
		return nullptr;
	}

	for (const FItemTextureEntry& Entry : Textures)
	{
		if (Entry.TextureRoleTag.MatchesTagExact(RoleTag) || Entry.TextureRoleTag.MatchesTag(RoleTag))
		{
			return Entry.Texture;
		}
	}

	return nullptr;
}
