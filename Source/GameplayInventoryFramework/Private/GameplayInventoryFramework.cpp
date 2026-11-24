#include "GameplayInventoryFramework.h"

#if WITH_EDITOR
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Brushes/SlateImageBrush.h"
#include "Misc/Paths.h"

TSharedPtr<FSlateStyleSet> FGameplayInventoryFrameworkModule::StyleSetInstance = nullptr;

TSharedPtr<FSlateStyleSet> FGameplayInventoryFrameworkModule::GetStyleSet()
{
	return StyleSetInstance;
}
#endif

DEFINE_LOG_CATEGORY(LogGameplayInventoryFramework);

#define LOCTEXT_NAMESPACE "FGameplayInventoryFrameworkModule"

void FGameplayInventoryFrameworkModule::StartupModule()
{
#if WITH_EDITOR
	StyleSetInstance = MakeShared<FSlateStyleSet>("GameplayInventoryFrameworkStyle");

	FString ResourcesDir = FPaths::ProjectPluginsDir() / TEXT("GameplayInventoryFramework/Resources");
	if (!FPaths::DirectoryExists(ResourcesDir))
	{
		ResourcesDir = FPaths::EnginePluginsDir() / TEXT("Marketplace/GameplayInventoryFramework/Resources");
	}
	if (!FPaths::DirectoryExists(ResourcesDir))
	{
		ResourcesDir = FPaths::EnginePluginsDir() / TEXT("GameplayInventoryFramework/Resources");
	}

	if (FPaths::DirectoryExists(ResourcesDir))
	{
		StyleSetInstance->SetContentRoot(ResourcesDir);

		const FVector2D Icon16(16.0f, 16.0f);
		const FVector2D Icon64(64.0f, 64.0f);

		auto RegisterClassStyle = [&](const FString& ClassName)
		{
			StyleSetInstance->Set(FName(*FString::Printf(TEXT("ClassIcon.%s"), *ClassName)),
				new FSlateImageBrush(StyleSetInstance->RootToContentDir(FString::Printf(TEXT("Icons/%s_16"), *ClassName), TEXT(".png")), Icon16));
			StyleSetInstance->Set(FName(*FString::Printf(TEXT("ClassThumbnail.%s"), *ClassName)),
				new FSlateImageBrush(StyleSetInstance->RootToContentDir(FString::Printf(TEXT("Icons/%s_64"), *ClassName), TEXT(".png")), Icon64));
		};

		// 1. Components
		RegisterClassStyle(TEXT("InventoryComponent"));
		RegisterClassStyle(TEXT("InventoryManagerComponent"));
		RegisterClassStyle(TEXT("ItemComponent"));

		// 2. Definitions & Instances
		RegisterClassStyle(TEXT("InventoryItemDefinition"));
		RegisterClassStyle(TEXT("InventoryItemInstance"));

		// 3. Fragments
		RegisterClassStyle(TEXT("InventoryItemFragment"));
		RegisterClassStyle(TEXT("Fragment_UIInfo"));
		RegisterClassStyle(TEXT("Fragment_ItemStats"));
		RegisterClassStyle(TEXT("Fragment_Stacking"));
		RegisterClassStyle(TEXT("Fragment_Montage"));
		RegisterClassStyle(TEXT("Fragment_Attachment"));
		RegisterClassStyle(TEXT("Fragment_Audio"));
		RegisterClassStyle(TEXT("Fragment_VFX"));
		RegisterClassStyle(TEXT("Fragment_Mesh"));
		RegisterClassStyle(TEXT("Fragment_Material"));
		RegisterClassStyle(TEXT("Fragment_Texture"));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSetInstance.Get());
	}
#endif

	UE_LOG(LogGameplayInventoryFramework, Log, TEXT("GameplayInventoryFramework module started."));
}

void FGameplayInventoryFrameworkModule::ShutdownModule()
{
#if WITH_EDITOR
	if (StyleSetInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSetInstance.Get());
		StyleSetInstance.Reset();
	}
#endif

	UE_LOG(LogGameplayInventoryFramework, Log, TEXT("GameplayInventoryFramework module shut down."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameplayInventoryFrameworkModule, GameplayInventoryFramework)
