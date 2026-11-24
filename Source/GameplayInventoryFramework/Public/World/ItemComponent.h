#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Definitions/InventoryItemDefinition.h"
#include "Instances/InventoryItemInstance.h"
#include "ItemComponent.generated.h"

class UInventoryComponent;

// --- Dynamic Multicast Delegates for Blueprint listening ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreItemAddedDelegate, UInventoryComponent*, TargetInv);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAddedDelegate, UInventoryComponent*, TargetInv, UInventoryItemInstance*, ItemInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostItemAddedDelegate, UInventoryComponent*, TargetInv, UInventoryItemInstance*, ItemInstance);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPreItemRemovedDelegate, UInventoryComponent*, SourceInv);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedDelegate, UInventoryComponent*, SourceInv, UInventoryItemInstance*, ItemInstance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostItemRemovedDelegate, UInventoryComponent*, SourceInv, AActor*, DroppedActor);

// --- Native Multicast Delegates for C++ and Unit Test listening ---
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreItemAddedNative, UInventoryComponent*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAddedNative, UInventoryComponent*, UInventoryItemInstance*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPostItemAddedNative, UInventoryComponent*, UInventoryItemInstance*);

USTRUCT(BlueprintType)
struct GAMEPLAYINVENTORYFRAMEWORK_API FWorldItemPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<const UInventoryItemDefinition> ItemDef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<UInventoryItemDefinition> ItemDefClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UInventoryItemInstance> ItemInstance = nullptr;
};

/**
 * Universal component that allows ANY actor to represent a physical world item or loot drop.
 * Integrates with UWorldItemSubsystem for spatial indexing and provides dual-channel pickup/drop lifecycles.
 */
UCLASS(ClassGroup = (Inventory), meta = (BlueprintSpawnableComponent))
class GAMEPLAYINVENTORYFRAMEWORK_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UItemComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

	// --- Initializers ---
	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	void InitFromInstance(UInventoryItemInstance* InInstance, int32 InCount = 1);

	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	void InitFromDefinition(const UInventoryItemDefinition* InDef, int32 InCount = 1);

	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	void InitFromDefinitionClass(TSubclassOf<UInventoryItemDefinition> InDefClass, int32 InCount = 1);

	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	void SetPayload(const UInventoryItemDefinition* InDef, int32 InCount, UInventoryItemInstance* InInstance = nullptr);

	// --- Extraction / Pickup Lifecycle Executions ---
	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	bool ExecutePickup(UInventoryComponent* TargetInventory);

	UFUNCTION(BlueprintCallable, Category = "WorldItem")
	bool ExtractItem(UInventoryItemInstance*& OutInstance, int32& OutCount);

	UFUNCTION(BlueprintPure, Category = "WorldItem")
	bool IsLooted() const { return bIsLooted; }

	UFUNCTION(BlueprintPure, Category = "WorldItem")
	const FWorldItemPayload& GetPayload() const { return Payload; }

	UFUNCTION(BlueprintPure, Category = "WorldItem")
	FVector GetItemWorldLocation() const;

	UFUNCTION(BlueprintPure, Category = "WorldItem")
	bool MatchesFilterQuery(const FGameplayTagQuery& FilterQuery) const;

	// --- Dynamic Delegates (Blueprint) ---
	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnPreItemAddedDelegate OnPreItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnItemAddedDelegate OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnPostItemAddedDelegate OnPostItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnPreItemRemovedDelegate OnPreItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnItemRemovedDelegate OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "WorldItem|Delegates")
	FOnPostItemRemovedDelegate OnPostItemRemoved;

	// --- Native Delegates (C++) ---
	FOnPreItemAddedNative OnPreItemAddedNative;
	FOnItemAddedNative OnItemAddedNative;
	FOnPostItemAddedNative OnPostItemAddedNative;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Payload, EditAnywhere, Category = "WorldItem")
	FWorldItemPayload Payload;

	UFUNCTION()
	void OnRep_Payload();

	UPROPERTY(ReplicatedUsing = OnRep_IsLooted)
	bool bIsLooted = false;

	UFUNCTION()
	void OnRep_IsLooted();
};
