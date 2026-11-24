#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Types/InventoryTypes.h"
#include "Definitions/InventoryItemDefinition.h"
#include "InventoryItemInstance.generated.h"

/**
 * Server-authoritative runtime instance of an item.
 * Zero subobject allocations: dynamic stats (ammo, reserve ammo, durability, serial numbers, attachments)
 * are stored directly in a flat replicated DynamicParameters bag.
 * Static fragments remain strictly read-only on the definition data asset.
 */
UCLASS(BlueprintType)
class GAMEPLAYINVENTORYFRAMEWORK_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemInstance();

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Initializes the instance with an item definition class (Internal C++ Constructor) */
	void Init(TSubclassOf<UInventoryItemDefinition> InItemDefClass);

	/** Initializes the instance with an item definition object (Internal C++ Constructor) */
	void InitFromObject(const UInventoryItemDefinition* InItemDef);

	/** Gets the item definition class */
	UFUNCTION(BlueprintPure, Category = "Item|Instance")
	TSubclassOf<UInventoryItemDefinition> GetItemDefClass() const { return ItemDefClass; }

	/** Gets the ItemDefinition CDO (Default Object) for reading static fragments and properties */
	UFUNCTION(BlueprintPure, Category = "Item|Instance")
	const UInventoryItemDefinition* GetItemDef() const;

	// --- Dynamic Tags (RepNotify) ---

	/** Adds a dynamic gameplay tag to this item instance */
	UFUNCTION(BlueprintCallable, Category = "Item|Tags", meta = (Categories = "Item.State,Item.Modifier"))
	void AddDynamicTag(UPARAM(meta = (Categories = "Item.State,Item.Modifier")) FGameplayTag InTag);

	/** Removes a dynamic gameplay tag */
	UFUNCTION(BlueprintCallable, Category = "Item|Tags", meta = (Categories = "Item.State,Item.Modifier"))
	void RemoveDynamicTag(UPARAM(meta = (Categories = "Item.State,Item.Modifier")) FGameplayTag InTag);

	/** Checks if this instance has the specified dynamic tag */
	UFUNCTION(BlueprintPure, Category = "Item|Tags", meta = (Categories = "Item.State,Item.Modifier"))
	bool HasDynamicTag(UPARAM(meta = (Categories = "Item.State,Item.Modifier")) FGameplayTag InTag) const;

	/** Gets all dynamic gameplay tags on this instance */
	UFUNCTION(BlueprintPure, Category = "Item|Tags")
	const FGameplayTagContainer& GetDynamicTags() const { return DynamicTags; }

	/** Delegate fired when dynamic tags change */
	UPROPERTY(BlueprintAssignable, Category = "Item|Events")
	FOnItemDynamicTagsChanged OnDynamicTagsChanged;

	// --- Dynamic Parameter Bag (Int, Float, Bool, String, Tag, TagContainer) ---

	// Typed Setters (Server Authoritative)
	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamInt(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamFloat(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, float Value);

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamBool(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, bool Value);

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamString(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, const FString& Value);

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamTag(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, FGameplayTag Value);

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	void SetParamTagContainer(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, const FGameplayTagContainer& Value);

	// Generic Parameter Setter (C++ helper)
	void SetParameter(const FItemDynamicParam& Parameter);

	// Typed Getters (Read Anywhere)
	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	int32 GetParamInt(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, int32 DefaultValue = 0) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	float GetParamFloat(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, float DefaultValue = 0.0f) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	bool GetParamBool(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, bool DefaultValue = false) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	FString GetParamString(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, const FString& DefaultValue = TEXT("")) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	FGameplayTag GetParamTag(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag, FGameplayTag DefaultValue = FGameplayTag()) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	FGameplayTagContainer GetParamTagContainer(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag) const;

	// Generic Parameter Query
	UFUNCTION(BlueprintPure, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	bool HasParameter(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "Item|Parameters")
	bool GetParameter(FGameplayTag Tag, FItemDynamicParam& OutParameter) const;

	UFUNCTION(BlueprintCallable, Category = "Item|Parameters", meta = (Categories = "Item.Param,Item.Stat"))
	bool RemoveParameter(UPARAM(meta = (Categories = "Item.Param,Item.Stat")) FGameplayTag Tag);

	// Backward Compatibility Forwarders for Dynamic Stats
	UFUNCTION(BlueprintPure, Category = "Item|Stats", meta = (Categories = "Item.Stat"))
	float GetDynamicFloat(UPARAM(meta = (Categories = "Item.Stat")) FGameplayTag StatTag, float DefaultValue = 0.0f) const
	{
		return GetParamFloat(StatTag, DefaultValue);
	}

	UFUNCTION(BlueprintCallable, Category = "Item|Stats", meta = (Categories = "Item.Stat"))
	void SetDynamicFloat(UPARAM(meta = (Categories = "Item.Stat")) FGameplayTag StatTag, float Value)
	{
		SetParamFloat(StatTag, Value);
	}

	UPROPERTY(BlueprintAssignable, Category = "Item|Events")
	FOnItemDynamicStatChanged OnDynamicStatChanged;

	// Main Parameter Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Item|Events")
	FOnItemParameterChanged OnParameterChanged;

	// --- Fragment Convenience Forwarders ---
	UFUNCTION(BlueprintPure, Category = "Item|Instance", meta = (Categories = "Item.Fragment"))
	const UInventoryItemFragment* FindFragmentByTag(UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "Item|Instance", meta = (Categories = "Item.Fragment"))
	TArray<UInventoryItemFragment*> FindFragmentsByTag(UPARAM(meta = (Categories = "Item.Fragment")) FGameplayTag Tag, bool bExactMatch = false) const;

	template<typename T>
	const T* FindFragmentByClass() const
	{
		if (const UInventoryItemDefinition* Def = GetItemDef())
		{
			return Def->FindFragmentByClass<T>();
		}
		return nullptr;
	}

	template<typename T>
	TArray<const T*> FindFragmentsByClass() const
	{
		if (const UInventoryItemDefinition* Def = GetItemDef())
		{
			return Def->FindFragmentsByClass<T>();
		}
		return {};
	}

	UFUNCTION(BlueprintPure, Category = "Item|Instance", meta = (DeterminesOutputType = "FragmentClass"))
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	UFUNCTION(BlueprintPure, Category = "Item|Instance")
	TArray<UInventoryItemFragment*> FindFragmentsByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

protected:
	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemDefinition> ItemDefClass;

	UPROPERTY(Replicated)
	TObjectPtr<const UInventoryItemDefinition> ItemDefObject;

	UPROPERTY(ReplicatedUsing = OnRep_DynamicTags)
	FGameplayTagContainer DynamicTags;

	UFUNCTION()
	void OnRep_DynamicTags();

	/** Dynamic parameters bag replicated via RepNotify to clients */
	UPROPERTY(ReplicatedUsing = OnRep_DynamicParameters)
	TArray<FItemDynamicParam> DynamicParameters;

	UFUNCTION()
	void OnRep_DynamicParameters();
};
