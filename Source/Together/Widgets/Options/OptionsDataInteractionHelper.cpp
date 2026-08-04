#include "OptionsDataInteractionHelper.h"

#include "Settings/UserSettings/UserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(
	const FName InSettingId,
	const FString& InDefaultValue,
	const bool bInIsNativeSetting) :
	CachedWeakUserSettings(UUserSettings::Get()),
	SettingId(InSettingId),
	DefaultValue(InDefaultValue),
	bIsNativeSetting(bInIsNativeSetting) {}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	if (const UUserSettings* UserSettings = CachedWeakUserSettings.Get())
	{
		return UserSettings->GetSetting(SettingId, DefaultValue, bIsNativeSetting);
	}

	return DefaultValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue) const
{
	if (UUserSettings* UserSettings = CachedWeakUserSettings.Get())
	{
		UserSettings->SetSetting(SettingId, InStringValue, bIsNativeSetting);
	}
}
