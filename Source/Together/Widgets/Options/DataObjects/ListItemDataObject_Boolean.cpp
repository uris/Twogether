// Uris - All Rights Reserved


#include "ListItemDataObject_Boolean.h"

void UListItemDataObject_Boolean::AddDynamicSetting(const FBoolSetting& InSetting)
{
	if (InSetting.TrueDisplayName.IsEmpty() || InSetting.FalseDisplayName.IsEmpty())
	{
		UE_LOG(LogTemp,
		       Warning,
		       TEXT("Boolean setting '%s' requires display names for both true and false."),
		       *GetDataId().ToString());
		return;
	}

	Settings = CreateBoolSettings(InSetting);
}

void UListItemDataObject_Boolean::OnDataObjectInitialized()
{
	EnsureBoolSettings();

	const FString ConfiguredDefault = GetDefaultValueAsString();

	if (ConfiguredDefault.Equals(TEXT("true"), ESearchCase::IgnoreCase))
	{
		SetDefaultValueFromString(TEXT("true"));
	}
	else if (ConfiguredDefault.Equals(TEXT("false"), ESearchCase::IgnoreCase))
	{
		SetDefaultValueFromString(TEXT("false"));
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Boolean setting '%s' has invalid default '%s'; using 'false'."),
			*GetDataId().ToString(),
			*ConfiguredDefault);
		SetDefaultValueFromString(TEXT("false"));
	}

	Super::OnDataObjectInitialized();
}

void UListItemDataObject_Boolean::EnsureBoolSettings()
{
	const bool bHasFalse = Settings.ContainsByPredicate(
		[](const FStringSetting& Setting)
		{
			return Setting.Value.Equals(TEXT("false"), ESearchCase::IgnoreCase);
		});
	const bool bHasTrue = Settings.ContainsByPredicate(
		[](const FStringSetting& Setting)
		{
			return Setting.Value.Equals(TEXT("true"), ESearchCase::IgnoreCase);
		});

	if (!bHasFalse || !bHasTrue)
	{
		Settings = CreateBoolSettings(FBoolSetting());
	}
}

TArray<FStringSetting> UListItemDataObject_Boolean::CreateBoolSettings(const FBoolSetting& InSetting) const
{
	TArray<FStringSetting> BoolSettings;

	BoolSettings.Emplace(GetDataId(), InSetting.FalseDisplayName, TEXT("false"));
	BoolSettings.Emplace(GetDataId(), InSetting.TrueDisplayName, TEXT("true"));

	return BoolSettings;
}
