// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "GameplayInventoryFramework.h"

#if WITH_EDITOR
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#endif

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGIFSlateStyleRegistrationTest,
	"GameplayInventory.Unit.Resources.SlateStyleRegistration",
	EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter
)

bool FGIFSlateStyleRegistrationTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	TSharedPtr<FSlateStyleSet> StyleSet = FGameplayInventoryFrameworkModule::GetStyleSet();
	TestTrue(TEXT("StyleSet should be initialized in editor builds"), StyleSet.IsValid());

	if (StyleSet.IsValid())
	{
		TestEqual(TEXT("StyleSetName should match GameplayInventoryFrameworkStyle"),
			StyleSet->GetStyleSetName(), FName("GameplayInventoryFrameworkStyle"));

		const TCHAR* TargetClasses[] = {
			TEXT("InventoryComponent"),
			TEXT("InventoryManagerComponent"),
			TEXT("ItemComponent"),
			TEXT("InventoryItemDefinition"),
			TEXT("InventoryItemInstance"),
			TEXT("InventoryItemFragment"),
			TEXT("Fragment_UIInfo"),
			TEXT("Fragment_ItemStats"),
			TEXT("Fragment_Stacking"),
			TEXT("Fragment_Montage"),
			TEXT("Fragment_Attachment"),
			TEXT("Fragment_Audio"),
			TEXT("Fragment_VFX"),
			TEXT("Fragment_Mesh"),
			TEXT("Fragment_Material"),
			TEXT("Fragment_Texture")
		};

		for (const TCHAR* ClassName : TargetClasses)
		{
			FString IconName = FString::Printf(TEXT("ClassIcon.%s"), ClassName);
			const FSlateBrush* IconBrush = StyleSet->GetBrush(FName(*IconName));
			TestNotNull(FString::Printf(TEXT("ClassIcon brush for %s must be registered"), ClassName), IconBrush);

			FString ThumbName = FString::Printf(TEXT("ClassThumbnail.%s"), ClassName);
			const FSlateBrush* ThumbBrush = StyleSet->GetBrush(FName(*ThumbName));
			TestNotNull(FString::Printf(TEXT("ClassThumbnail brush for %s must be registered"), ClassName), ThumbBrush);
		}
	}
#endif

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
