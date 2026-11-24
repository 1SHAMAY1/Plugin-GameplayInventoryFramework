#include "Instances/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

UInventoryItemInstance::UInventoryItemInstance()
{
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, ItemDefClass);
	DOREPLIFETIME(UInventoryItemInstance, ItemDefObject);
	DOREPLIFETIME(UInventoryItemInstance, DynamicTags);
	DOREPLIFETIME(UInventoryItemInstance, DynamicParameters);
}

void UInventoryItemInstance::Init(TSubclassOf<UInventoryItemDefinition> InItemDefClass)
{
	ItemDefClass = InItemDefClass;
	if (ItemDefClass)
	{
		ItemDefObject = GetDefault<UInventoryItemDefinition>(ItemDefClass);
	}
}

void UInventoryItemInstance::InitFromObject(const UInventoryItemDefinition* InItemDef)
{
	if (InItemDef)
	{
		ItemDefObject = InItemDef;
		ItemDefClass = InItemDef->GetClass();
	}
}

const UInventoryItemDefinition* UInventoryItemInstance::GetItemDef() const
{
	if (ItemDefObject)
	{
		return ItemDefObject.Get();
	}

	if (ItemDefClass)
	{
		return GetDefault<UInventoryItemDefinition>(ItemDefClass);
	}

	return nullptr;
}

// --- Dynamic Tags ---

void UInventoryItemInstance::AddDynamicTag(FGameplayTag InTag)
{
	if (!InTag.IsValid())
	{
		return;
	}

	if (!DynamicTags.HasTagExact(InTag))
	{
		DynamicTags.AddTag(InTag);
		OnDynamicTagsChanged.Broadcast(DynamicTags);
	}
}

void UInventoryItemInstance::RemoveDynamicTag(FGameplayTag InTag)
{
	if (!InTag.IsValid())
	{
		return;
	}

	if (DynamicTags.HasTagExact(InTag))
	{
		DynamicTags.RemoveTag(InTag);
		OnDynamicTagsChanged.Broadcast(DynamicTags);
	}
}

bool UInventoryItemInstance::HasDynamicTag(FGameplayTag InTag) const
{
	return DynamicTags.HasTag(InTag);
}

void UInventoryItemInstance::OnRep_DynamicTags()
{
	OnDynamicTagsChanged.Broadcast(DynamicTags);
}

// --- Dynamic Parameters ---

void UInventoryItemInstance::SetParamInt(FGameplayTag Tag, int32 Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::Int;
			Param.IntValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::Int;
	NewParam.IntValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
}

void UInventoryItemInstance::SetParamFloat(FGameplayTag Tag, float Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::Float;
			Param.FloatValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			OnDynamicStatChanged.Broadcast(Tag, Value);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::Float;
	NewParam.FloatValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
	OnDynamicStatChanged.Broadcast(Tag, Value);
}

void UInventoryItemInstance::SetParamBool(FGameplayTag Tag, bool Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::Bool;
			Param.BoolValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::Bool;
	NewParam.BoolValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
}

void UInventoryItemInstance::SetParamString(FGameplayTag Tag, const FString& Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::String;
			Param.StringValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::String;
	NewParam.StringValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
}

void UInventoryItemInstance::SetParamTag(FGameplayTag Tag, FGameplayTag Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::Tag;
			Param.TagValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::Tag;
	NewParam.TagValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
}

void UInventoryItemInstance::SetParamTagContainer(FGameplayTag Tag, const FGameplayTagContainer& Value)
{
	if (!Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			Param.Type = EItemDynamicParamType::TagContainer;
			Param.TagContainerValue = Value;
			OnParameterChanged.Broadcast(this, Tag);
			return;
		}
	}

	FItemDynamicParam NewParam;
	NewParam.Tag = Tag;
	NewParam.Type = EItemDynamicParamType::TagContainer;
	NewParam.TagContainerValue = Value;
	DynamicParameters.Add(NewParam);
	OnParameterChanged.Broadcast(this, Tag);
}

void UInventoryItemInstance::SetParameter(const FItemDynamicParam& Parameter)
{
	if (!Parameter.Tag.IsValid())
	{
		return;
	}

	for (FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Parameter.Tag)
		{
			Param = Parameter;
			OnParameterChanged.Broadcast(this, Parameter.Tag);
			return;
		}
	}

	DynamicParameters.Add(Parameter);
	OnParameterChanged.Broadcast(this, Parameter.Tag);
}

int32 UInventoryItemInstance::GetParamInt(FGameplayTag Tag, int32 DefaultValue) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::Int)
		{
			return Param.IntValue;
		}
	}
	return DefaultValue;
}

float UInventoryItemInstance::GetParamFloat(FGameplayTag Tag, float DefaultValue) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::Float)
		{
			return Param.FloatValue;
		}
	}
	return DefaultValue;
}

bool UInventoryItemInstance::GetParamBool(FGameplayTag Tag, bool DefaultValue) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::Bool)
		{
			return Param.BoolValue;
		}
	}
	return DefaultValue;
}

FString UInventoryItemInstance::GetParamString(FGameplayTag Tag, const FString& DefaultValue) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::String)
		{
			return Param.StringValue;
		}
	}
	return DefaultValue;
}

FGameplayTag UInventoryItemInstance::GetParamTag(FGameplayTag Tag, FGameplayTag DefaultValue) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::Tag)
		{
			return Param.TagValue;
		}
	}
	return DefaultValue;
}

FGameplayTagContainer UInventoryItemInstance::GetParamTagContainer(FGameplayTag Tag) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag && Param.Type == EItemDynamicParamType::TagContainer)
		{
			return Param.TagContainerValue;
		}
	}
	return FGameplayTagContainer();
}

bool UInventoryItemInstance::HasParameter(FGameplayTag Tag) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			return true;
		}
	}
	return false;
}

bool UInventoryItemInstance::GetParameter(FGameplayTag Tag, FItemDynamicParam& OutParameter) const
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		if (Param.Tag == Tag)
		{
			OutParameter = Param;
			return true;
		}
	}
	return false;
}

bool UInventoryItemInstance::RemoveParameter(FGameplayTag Tag)
{
	for (int32 Index = 0; Index < DynamicParameters.Num(); ++Index)
	{
		if (DynamicParameters[Index].Tag == Tag)
		{
			DynamicParameters.RemoveAt(Index);
			OnParameterChanged.Broadcast(this, Tag);
			return true;
		}
	}
	return false;
}

void UInventoryItemInstance::OnRep_DynamicParameters()
{
	for (const FItemDynamicParam& Param : DynamicParameters)
	{
		OnParameterChanged.Broadcast(this, Param.Tag);
		if (Param.Type == EItemDynamicParamType::Float)
		{
			OnDynamicStatChanged.Broadcast(Param.Tag, Param.FloatValue);
		}
	}
}

// --- Fragment Forwarders ---

const UInventoryItemFragment* UInventoryItemInstance::FindFragmentByTag(FGameplayTag Tag) const
{
	if (const UInventoryItemDefinition* Def = GetItemDef())
	{
		return Def->FindFragmentByTag(Tag);
	}
	return nullptr;
}

TArray<UInventoryItemFragment*> UInventoryItemInstance::FindFragmentsByTag(FGameplayTag Tag, bool bExactMatch) const
{
	if (const UInventoryItemDefinition* Def = GetItemDef())
	{
		return Def->FindFragmentsByTag(Tag, bExactMatch);
	}
	return {};
}

const UInventoryItemFragment* UInventoryItemInstance::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (const UInventoryItemDefinition* Def = GetItemDef())
	{
		return Def->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

TArray<UInventoryItemFragment*> UInventoryItemInstance::FindFragmentsByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (const UInventoryItemDefinition* Def = GetItemDef())
	{
		return Def->FindFragmentsByClass(FragmentClass);
	}
	return {};
}
