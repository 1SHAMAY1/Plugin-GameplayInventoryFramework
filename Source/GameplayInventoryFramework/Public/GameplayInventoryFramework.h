#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#if WITH_EDITOR
class FSlateStyleSet;
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogGameplayInventoryFramework, Log, All);

class GAMEPLAYINVENTORYFRAMEWORK_API FGameplayInventoryFrameworkModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	static TSharedPtr<FSlateStyleSet> GetStyleSet();
private:
	static TSharedPtr<FSlateStyleSet> StyleSetInstance;
#endif
};
