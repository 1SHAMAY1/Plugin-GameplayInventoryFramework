#include "Subsystems/WorldItemSubsystem.h"
#include "World/ItemComponent.h"

void UWorldItemSubsystem::Deinitialize()
{
	WorldItems.Empty();
	Super::Deinitialize();
}

void UWorldItemSubsystem::RegisterWorldItem(UItemComponent* ItemComp)
{
	if (ItemComp && !WorldItems.Contains(ItemComp))
	{
		WorldItems.Add(ItemComp);
	}
}

void UWorldItemSubsystem::UnregisterWorldItem(UItemComponent* ItemComp)
{
	if (ItemComp)
	{
		WorldItems.Remove(ItemComp);
	}
}

int32 UWorldItemSubsystem::GetRegisteredItemCount() const
{
	int32 Count = 0;
	for (const TWeakObjectPtr<UItemComponent>& WeakItem : WorldItems)
	{
		if (WeakItem.IsValid())
		{
			Count++;
		}
	}
	return Count;
}

TArray<UItemComponent*> UWorldItemSubsystem::FindNearbyItems(FVector Center, float Radius, const FGameplayTagQuery& FilterQuery) const
{
	TArray<UItemComponent*> Results;
	const float RadiusSq = FMath::Square(Radius);

	for (const TWeakObjectPtr<UItemComponent>& WeakItem : WorldItems)
	{
		if (UItemComponent* ItemComp = WeakItem.Get())
		{
			const FVector ItemLocation = ItemComp->GetItemWorldLocation();
			if (FVector::DistSquared(Center, ItemLocation) <= RadiusSq)
			{
				if (FilterQuery.IsEmpty() || ItemComp->MatchesFilterQuery(FilterQuery))
				{
					Results.Add(ItemComp);
				}
			}
		}
	}

	return Results;
}
