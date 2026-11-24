#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "Fragments/Fragment_Montage.h"
#include "InventoryFunctionLibrary.generated.h"

class USceneComponent;
class UInventoryComponent;
class UInventoryManagerComponent;
class UItemRegistrySubsystem;
class UWorldItemSubsystem;

/**
 * Universal Blueprint Function Library for Gameplay Inventory Framework.
 * Provides designer-facing attachment, fragment query, montage lookup, and dynamic stat helpers.
 */
UCLASS()
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// --- Native Visual Attachment Helpers (Zero Component Bloat) ---

	/** Attaches an item visual representation to the target component based on its UFragment_Attachment configuration */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Attachment")
	static USceneComponent* AttachItemVisual(USceneComponent* TargetMesh, const UInventoryItemInstance* ItemInstance);

	/** Detaches all visuals attached to a specific socket on TargetMesh */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Attachment")
	static void DetachItemVisual(USceneComponent* TargetMesh, FName SocketName);

	// --- Fragment & Static Definition Queries ---

	/** Finds the first fragment matching the specified tag on an item definition */
	UFUNCTION(BlueprintPure, Category = "Inventory|Fragments", meta = (Categories = "Item.Fragment"))
	static const UInventoryItemFragment* GetFragmentByTag(const UInventoryItemDefinition* ItemDef, UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag FragmentTag);

	/** Checks if an item definition contains a fragment matching the specified tag */
	UFUNCTION(BlueprintPure, Category = "Inventory|Fragments", meta = (Categories = "Item.Fragment"))
	static bool HasFragmentTag(const UInventoryItemDefinition* ItemDef, UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag FragmentTag);

	/** Finds an animation montage entry matching the given action tag on an item definition */
	UFUNCTION(BlueprintPure, Category = "Inventory|Animation", meta = (Categories = "Animation.Montage"))
	static bool GetMontageByTag(const UInventoryItemDefinition* ItemDef, UPARAM(meta = (Categories = "Animation.Montage")) FGameplayTag MontageTag, FItemMontageEntry& OutEntry);

	// --- Dynamic Stat & Parameter Accessors ---

	UFUNCTION(BlueprintPure, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static float GetItemDynamicFloat(const UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, float DefaultValue = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static void SetItemDynamicFloat(UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, float Value);

	UFUNCTION(BlueprintPure, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static int32 GetItemDynamicInt(const UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, int32 DefaultValue = 0);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static void SetItemDynamicInt(UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, int32 Value);

	UFUNCTION(BlueprintPure, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static bool GetItemDynamicBool(const UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, bool DefaultValue = false);

	UFUNCTION(BlueprintCallable, Category = "Inventory|Stats", meta = (Categories = "Item.Stat,Item.Param"))
	static void SetItemDynamicBool(UInventoryItemInstance* ItemInstance, UPARAM(meta = (Categories = "Item.Stat,Item.Param")) FGameplayTag StatTag, bool Value);

	// --- Subsystem Accessors ---

	UFUNCTION(BlueprintPure, Category = "Inventory|Subsystems", meta = (WorldContext = "WorldContextObject"))
	static UItemRegistrySubsystem* GetItemRegistrySubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Inventory|Subsystems", meta = (WorldContext = "WorldContextObject"))
	static UWorldItemSubsystem* GetWorldItemSubsystem(const UObject* WorldContextObject);
};
