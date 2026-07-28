// Uris - All Rights Reserved


#include "Widgets/Options/DataObjects/ListItemDataObject_String.h"

#include "Distributions/Distribution.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"

void UListItemDataObject_String::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (Settings.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unable to initialize string setting '%s': no available values were configured."),
			*GetDataId().ToString());
		CurrentSetting.DisplayName = FText::FromString("Invalid Option");
		CurrentSetting.Value = TEXT("Invalid Value");
		return;
	}

	FString ValueToUse = Settings[0].Value;
	if (HasDefaultValue() && GetSettingIndexByValue(GetDefaultValueAsString()) != INDEX_NONE)
	{
		ValueToUse = GetDefaultValueAsString();
	}

	if (DataDynamicGetter)
	{
		ValueToUse = DataDynamicGetter->GetValueAsString();
	}

	if (DidSetDisplayNameFromStringValue(ValueToUse))
	{
		return;
	}

	const FString FallbackValue =
		HasDefaultValue() && GetSettingIndexByValue(GetDefaultValueAsString()) != INDEX_NONE
			? GetDefaultValueAsString()
			: Settings[0].Value;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("String setting '%s' contains unavailable value '%s'; using '%s' instead."),
		*GetDataId().ToString(),
		*ValueToUse,
		*FallbackValue);

	DidSetDisplayNameFromStringValue(FallbackValue);

	// Repair a stale saved value so subsequent loads use a valid table option.
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(FallbackValue);
	}
}

bool UListItemDataObject_String::CanResetBackToDefault() const
{
	return HasDefaultValue() && CurrentSetting.Value != GetDefaultValueAsString();
}

bool UListItemDataObject_String::ResetToDefault()
{
	if (!CanResetBackToDefault())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to reset: no applicable default value."));
		return false;
	}

	if (!DataDynamicSetter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to reset: no dynamic setter."));
		return false;
	}

	if (!DidSetDisplayNameFromStringValue(GetDefaultValueAsString()))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unable to reset string setting '%s': default value '%s' is not available."),
			*GetDataId().ToString(),
			*GetDefaultValueAsString());
		return false;
	}

	DataDynamicSetter->SetValueFromString(CurrentSetting.Value);
	NotifyListDataModified(this, EOptionsListModifiedReason::ResetToDefault);

	return true;
}

void UListItemDataObject_String::OnRotatorInitiatedValueChange(const FText& InDisplayName)
{
	if (const int32 IndexOfText = GetSettingIndexByDisplayName(InDisplayName); IndexOfText != INDEX_NONE)
	{
		CurrentSetting = Settings[IndexOfText];
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentSetting.Value);
			NotifyListDataModified(this);
		}
	}
}

void UListItemDataObject_String::AddDynamicSetting(const FName& InSettingDataId,
                                                   const FText& InDisplayName,
                                                   const FString& InValue)
{
	AddDynamicSetting(FStringSetting(InSettingDataId, InDisplayName, InValue));
}

void UListItemDataObject_String::AddDynamicSetting(const FStringSetting& InSetting)
{
	const int32 FoundIndex = GetSettingIndexByValue(InSetting.Value);
	if (FoundIndex != INDEX_NONE)
	{
		UE_LOG(LogTemp,
		       Warning,
		       TEXT("Attempted to add duplicate value '%s' to string setting '%s'."),
		       *InSetting.Value,
		       *GetDataId().ToString());
		return;
	}

	Settings.Add(InSetting);
}

FText UListItemDataObject_String::CycleCurrentSetting(const EStringSettingDirection InDirection)
{
	// get current index and make sure its done
	const int32 CurrentIndex = GetSettingIndexByValue(CurrentSetting.Value);
	if (CurrentIndex == INDEX_NONE)
	{
		return FText::GetEmpty();
	}

	// set next index
	int32 NextIndex = InDirection == EStringSettingDirection::Next ? CurrentIndex + 1 : CurrentIndex - 1;

	// and make sure it cycles the array bounds
	if (NextIndex >= Settings.Num())
	{
		NextIndex = 0;
	}
	else if (NextIndex < 0)
	{
		NextIndex = Settings.Num() - 1;
	}

	// set it to current values
	CurrentSetting = Settings[NextIndex];
	const bool bDidUpdate = DidSetDisplayNameFromStringValue(CurrentSetting.Value);

	// save the latest value
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentSetting.Value);
		NotifyListDataModified(this);
	}

	// return the display name for convenience
	return bDidUpdate ? CurrentSetting.DisplayName : FText::GetEmpty();
}

TArray<FText> UListItemDataObject_String::GetAvailableDisplayOptionsArray()
{
	// reduce the settings array to a text array containing the display names only

	TArray<FText> AvailableOptions;
	AvailableOptions.Reserve(Settings.Num());

	for (const FStringSetting& Setting : Settings)
	{
		AvailableOptions.Add(Setting.DisplayName);
	}

	return AvailableOptions;
}

bool UListItemDataObject_String::DidSetDisplayNameFromStringValue(const FString& InStringValue)
{
	const int32 FoundIndex = GetSettingIndexByValue(InStringValue);

	if (FoundIndex == INDEX_NONE)
	{
		return false;
	}

	CurrentSetting = Settings[FoundIndex];
	return !CurrentSetting.DisplayName.IsEmpty();
}

int32 UListItemDataObject_String::GetSettingIndexByValue(const FString& InStringValue) const
{
	return Settings.IndexOfByPredicate(
		[&InStringValue](const FStringSetting& Setting)
		{
			return Setting.Value == InStringValue;
		});
}

int32 UListItemDataObject_String::GetSettingIndexByDisplayName(const FText& InDisplayName) const
{
	return Settings.IndexOfByPredicate(
		[&InDisplayName](const FStringSetting& Setting)
		{
			return Setting.DisplayName.ToString() == InDisplayName.ToString();
		});
}
