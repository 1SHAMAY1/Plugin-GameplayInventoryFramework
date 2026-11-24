#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryTypes.generated.h"

class UInventoryItemInstance;
class UInventoryItemDefinition;

/** Supported data types in the dynamic parameter bag */
UENUM(BlueprintType)
enum class EItemDynamicParamType : uint8
{
	None,
	Int,
	Float,
	Bool,
	String,
	Tag,
	TagContainer
};

/**
 * Replicated dynamic parameter entry.
 * Solves all runtime item state (ammo, reserve ammo, durability, serial numbers, attachments)
 * with zero UObject runtime allocations and zero GC overhead.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FItemDynamicParam
{
	GENERATED_BODY()

	FItemDynamicParam()
		: Type(EItemDynamicParamType::None)
		, IntValue(0)
		, FloatValue(0.0f)
		, BoolValue(false)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta = (Categories = "Item.Param,Item.Stat"))
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	EItemDynamicParamType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	int32 IntValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float FloatValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	bool BoolValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FString StringValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter", meta = (Categories = "Item"))
	FGameplayTag TagValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGameplayTagContainer TagContainerValue;

	bool operator==(const FItemDynamicParam& Other) const
	{
		return Tag == Other.Tag && Type == Other.Type &&
			IntValue == Other.IntValue &&
			FMath::IsNearlyEqual(FloatValue, Other.FloatValue) &&
			BoolValue == Other.BoolValue &&
			StringValue == Other.StringValue &&
			TagValue == Other.TagValue &&
			TagContainerValue == Other.TagContainerValue;
	}
};

/**
 * Replicated Tag-Float capacity limit rule entry.
 * Completely dynamic: supports Slots, Weight, Volume, Energy, or any game-specific capacity limit.
 */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FInventoryCapacityLimit
{
	GENERATED_BODY()

	FInventoryCapacityLimit()
		: Value(0.0f)
	{
	}

	FInventoryCapacityLimit(FGameplayTag InTag, float InValue)
		: LimitTag(InTag)
		, Value(InValue)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capacity", meta = (Categories = "Inventory.Capacity"))
	FGameplayTag LimitTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Capacity")
	float Value;

	bool operator==(const FInventoryCapacityLimit& Other) const
	{
		return LimitTag == Other.LimitTag && FMath::IsNearlyEqual(Value, Other.Value);
	}
};

/** Legacy dynamic stat entry for backward compatibility */
USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FDynamicStatEntry
{
	GENERATED_BODY()

	FDynamicStatEntry()
		: Value(0.0f)
	{
	}

	FDynamicStatEntry(FGameplayTag InTag, float InValue)
		: StatTag(InTag)
		, Value(InValue)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (Categories = "Item.Stat"))
	FGameplayTag StatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Value;

	bool operator==(const FDynamicStatEntry& Other) const
	{
		return StatTag == Other.StatTag && FMath::IsNearlyEqual(Value, Other.Value);
	}
};

/** Multicast dynamic delegate when an item instance's dynamic tags change */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDynamicTagsChanged, const FGameplayTagContainer&, NewTags);

/** Multicast dynamic delegate when an item instance's dynamic stat changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDynamicStatChanged, FGameplayTag, StatTag, float, NewValue);

/** Multicast dynamic delegate when any parameter changes on an item instance */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemParameterChanged, UInventoryItemInstance*, ItemInstance, FGameplayTag, ParamTag);

/** Multicast dynamic delegate when a capacity limit changes on an inventory container */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCapacityLimitChanged, FGameplayTag, CapacityTag, float, NewLimit);
