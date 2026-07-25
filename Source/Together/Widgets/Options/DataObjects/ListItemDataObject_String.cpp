// Uris - All Rights Reserved


#include "Widgets/Options/DataObjects/ListItemDataObject_String.h"

#include "Distributions/Distribution.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"

void UListItemDataObject_String::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	// if no settings are available, set current to invalid
	if (Settings.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Empty Settings - Unable To Initialize"));
		CurrentSetting.DisplayName = FText::FromString("Invalid Option");
		CurrentSetting.Value = FString("Invalid Value");
	}

	// otherwise default to the first entry in the array
	if (!Settings.IsEmpty())
	{
		CurrentSetting.Value = Settings[0].Value;
	}

	// check if there's a default value
	if (HasDefaultValue())
	{
		CurrentSetting.Value = GetDefaultValueAsString();
	}

	// check saved settings for a previously stored value
	if (DataDynamicGetter)
	{
		FString FoundSettingValue = DataDynamicGetter->GetValueAsString();
		if (!FoundSettingValue.IsEmpty())
		{
			CurrentSetting.Value = FoundSettingValue;
		}
	}

	// update display text and report issues updating
	const bool DidUpdateSetting = DidSetDisplayNameFromStringValue(CurrentSetting.Value);
	if (!DidUpdateSetting)
	{
		CurrentSetting.DisplayName = FText::FromString("Invalid Option");
		CurrentSetting.Value = FString("Invalid Value");
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

	CurrentSetting.Value = GetDefaultValueAsString();
	DidSetDisplayNameFromStringValue(CurrentSetting.Value);
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
	bool bIsValid = true;

	// get an index of then entry matching the same value
	const int32 FoundIndex = GetSettingIndexByValue(InSetting.Value);

	// check if the value exists for the same setting id
	if (FoundIndex != INDEX_NONE && Settings[FoundIndex].SettingDataId == InSetting.SettingDataId)
	{
		bIsValid = false;
	}

	// return is an invalid setting
	if (!bIsValid)
	{
		UE_LOG(LogTemp,
		       Warning,
		       TEXT("Attempted to add duplicate setting for %s: %s"),
		       *InSetting.SettingDataId.ToString(),
		       *InSetting.Value);
		return;
	}

	// add setting
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
	const bool bDidUpdate = DidSetDisplayNameFromStringValue(CurrentSetting.DisplayName.ToString());

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
		UE_LOG(LogTemp, Log, TEXT("Selection is Invalid - Unable To Set Option Display Name"));
		return false;
	}
	else
	{
		CurrentSetting = Settings[FoundIndex];
		return !Settings[FoundIndex].DisplayName.IsEmpty();
	}
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
