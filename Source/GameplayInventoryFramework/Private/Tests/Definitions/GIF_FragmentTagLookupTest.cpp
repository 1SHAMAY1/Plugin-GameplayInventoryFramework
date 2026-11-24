#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Fragments/Fragment_UIInfo.h"
#include "Fragments/Fragment_Montage.h"
#include "Fragments/Fragment_Stacking.h"
#include "Fragments/Fragment_Mesh.h"
#include "Fragments/Fragment_VFX.h"
#include "Fragments/Fragment_Audio.h"
#include "Fragments/Fragment_Texture.h"
#include "Fragments/Fragment_Material.h"
#include "Types/InventoryNativeTags.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGIFFragmentTagLookupTest, "GameplayInventory.Definitions.FragmentTagLookup", 
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FGIFFragmentTagLookupTest::RunTest(const FString& Parameters)
{
	UInventoryItemDefinition* ItemDef = NewObject<UInventoryItemDefinition>(GetTransientPackage());
	TestNotNull(TEXT("ItemDef must be valid"), ItemDef);
	if (!ItemDef)
	{
		return false;
	}

	// 1. Add UI Fragment
	UFragment_UIInfo* UIFrag = NewObject<UFragment_UIInfo>(ItemDef);
	UIFrag->FragmentTag = MGITags::Item_Fragment_UI;
	UIFrag->DisplayName = FText::FromString(TEXT("Iron Longsword"));
	ItemDef->AddFragment(UIFrag);

	// 1b. Add Secondary UI Fragment (tests plural queries)
	UFragment_UIInfo* UIFragSecondary = NewObject<UFragment_UIInfo>(ItemDef);
	UIFragSecondary->FragmentTag = MGITags::Item_Fragment_UI;
	UIFragSecondary->DisplayName = FText::FromString(TEXT("Blade of the Ancient King"));
	ItemDef->AddFragment(UIFragSecondary);

	// 2. Add Multi-Montage Fragment
	UFragment_Montage* MontageFrag = NewObject<UFragment_Montage>(ItemDef);
	MontageFrag->FragmentTag = MGITags::Item_Fragment_Montage;

	FItemMontageEntry AttackEntry;
	AttackEntry.MontageTag = MGITags::Animation_Montage_Attack;
	AttackEntry.PlayRate = 1.25f;
	MontageFrag->Montages.Add(AttackEntry);
	ItemDef->AddFragment(MontageFrag);

	// 3. Add Dedicated Stacking Fragment
	UFragment_Stacking* StackingFrag = NewObject<UFragment_Stacking>(ItemDef);
	StackingFrag->MaxStackCount = 50;
	StackingFrag->bCanSplit = true;
	StackingFrag->bCanCombine = true;
	ItemDef->AddFragment(StackingFrag);

	// 4. Add Dedicated Mesh Fragment with bIsSkeletal toggle
	UFragment_Mesh* MeshFrag = NewObject<UFragment_Mesh>(ItemDef);
	MeshFrag->bIsSkeletal = false;
	ItemDef->AddFragment(MeshFrag);

	// 5. Add Dedicated VFX Fragment with bIsNiagara toggle
	UFragment_VFX* VFXFrag = NewObject<UFragment_VFX>(ItemDef);
	FItemVFXEntry VFXEntry;
	VFXEntry.VFXTag = MGITags::VFX_Item_MuzzleFlash;
	VFXEntry.bIsNiagara = true;
	VFXEntry.bAutoActivate = true;
	VFXFrag->VFXEntries.Add(VFXEntry);
	ItemDef->AddFragment(VFXFrag);

	// 6. Add Dedicated Audio Fragment
	UFragment_Audio* AudioFrag = NewObject<UFragment_Audio>(ItemDef);
	FItemAudioEntry AudioEntry;
	AudioEntry.AudioTag = MGITags::Audio_Item_Fire;
	AudioEntry.VolumeMultiplier = 1.5f;
	AudioFrag->AudioEntries.Add(AudioEntry);
	ItemDef->AddFragment(AudioFrag);

	// Verification 1: UI Fragment exact tag query
	const UInventoryItemFragment* FoundUI = ItemDef->FindFragmentByTag(MGITags::Item_Fragment_UI);
	TestNotNull(TEXT("UI Fragment retrieved by tag"), FoundUI);
	TestEqual(TEXT("UI Fragment identity check"), FoundUI, static_cast<const UInventoryItemFragment*>(UIFrag));

	// Verification 2: Stacking Fragment & Definition Query
	const UFragment_Stacking* FoundStacking = ItemDef->FindFragmentByClass<UFragment_Stacking>();
	TestNotNull(TEXT("Stacking fragment retrieved by class"), FoundStacking);
	TestEqual(TEXT("GetMaxStackCount queries fragment"), ItemDef->GetMaxStackCount(), 50);

	// Verification 3: Mesh Fragment Toggle
	const UFragment_Mesh* FoundMesh = ItemDef->FindFragmentByClass<UFragment_Mesh>();
	TestNotNull(TEXT("Mesh fragment retrieved by class"), FoundMesh);
	if (FoundMesh)
	{
		TestFalse(TEXT("bIsSkeletal is false for static mesh"), FoundMesh->bIsSkeletal);
	}

	// Verification 4: VFX Fragment Toggle & Lookup
	const UFragment_VFX* FoundVFX = ItemDef->FindFragmentByClass<UFragment_VFX>();
	TestNotNull(TEXT("VFX fragment retrieved by class"), FoundVFX);
	if (FoundVFX)
	{
		const FItemVFXEntry* FoundVFXEntry = FoundVFX->FindVFXEntry(MGITags::VFX_Item_MuzzleFlash);
		TestNotNull(TEXT("MuzzleFlash VFX entry found"), FoundVFXEntry);
		if (FoundVFXEntry)
		{
			TestTrue(TEXT("bIsNiagara is toggled true"), FoundVFXEntry->bIsNiagara);
			TestTrue(TEXT("bAutoActivate is true"), FoundVFXEntry->bAutoActivate);
		}
	}

	// Verification 5: Audio Fragment Lookup
	const UFragment_Audio* FoundAudio = ItemDef->FindFragmentByClass<UFragment_Audio>();
	TestNotNull(TEXT("Audio fragment retrieved by class"), FoundAudio);
	if (FoundAudio)
	{
		const FItemAudioEntry* FoundAudioEntry = FoundAudio->FindAudioEntry(MGITags::Audio_Item_Fire);
		TestNotNull(TEXT("Fire Audio entry found"), FoundAudioEntry);
		if (FoundAudioEntry)
		{
			TestEqual(TEXT("Volume multiplier matches"), FoundAudioEntry->VolumeMultiplier, 1.5f);
		}
	}

	// Verification 6: Missing Tag Query returns nullptr safely
	const FGameplayTag MissingTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Fragment.NonExistentTag"), false);
	const UInventoryItemFragment* Missing = ItemDef->FindFragmentByTag(MissingTag);
	TestNull(TEXT("Non-existent tag must return nullptr"), Missing);

	// Verification 7: Plural FindFragmentsByClass (Blueprint & C++)
	TArray<UInventoryItemFragment*> UIFragmentsBP = ItemDef->FindFragmentsByClass(UFragment_UIInfo::StaticClass());
	TestEqual(TEXT("FindFragmentsByClass retrieves all UI fragments"), UIFragmentsBP.Num(), 2);

	TArray<const UFragment_UIInfo*> UIFragmentsNative = ItemDef->FindFragmentsByClass<UFragment_UIInfo>();
	TestEqual(TEXT("FindFragmentsByClass<T> retrieves all typed UI fragments"), UIFragmentsNative.Num(), 2);

	// Verification 8: Plural FindFragmentsByTag with hierarchical matching (MatchesTag)
	// UI fragments (2), Montage (1), Stacking (0 explicit tag or default), VFX (1), Audio (1)
	TArray<UInventoryItemFragment*> AllItemFragments = ItemDef->FindFragmentsByTag(MGITags::Item_Fragment, false);
	TestTrue(TEXT("Hierarchical FindFragmentsByTag retrieves matching child fragments"), AllItemFragments.Num() >= 4);

	// Verification 9: Plural FindFragmentsByTag with exact matching (MatchesTagExact)
	TArray<UInventoryItemFragment*> ExactRootFragments = ItemDef->FindFragmentsByTag(MGITags::Item_Fragment, true);
	TestEqual(TEXT("Exact matching on root tag returns 0 child fragments"), ExactRootFragments.Num(), 0);

	TArray<UInventoryItemFragment*> ExactUIFragments = ItemDef->FindFragmentsByTag(MGITags::Item_Fragment_UI, true);
	TestEqual(TEXT("Exact matching on Item.Fragment.UI returns 2 UI fragments"), ExactUIFragments.Num(), 2);

	// Verification 10: Instance convenience forwarders
	UInventoryItemInstance* TestInstance = NewObject<UInventoryItemInstance>(GetTransientPackage());
	TestInstance->InitFromObject(ItemDef);

	TArray<UInventoryItemFragment*> InstanceUIFrags = TestInstance->FindFragmentsByTag(MGITags::Item_Fragment_UI, true);
	TestEqual(TEXT("Instance FindFragmentsByTag forwards correctly"), InstanceUIFrags.Num(), 2);

	TArray<UInventoryItemFragment*> InstanceClassFrags = TestInstance->FindFragmentsByClass(UFragment_UIInfo::StaticClass());
	TestEqual(TEXT("Instance FindFragmentsByClass forwards correctly"), InstanceClassFrags.Num(), 2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
