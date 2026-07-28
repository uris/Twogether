#include "OptionsDataInteractionHelper.h"

#include "Settings/UserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(
	const FName InSettingId,
	const FString& InDefaultValue) :
	CachedWeakUserSettings(UUserSettings::Get()),
	SettingId(InSettingId),
	DefaultValue(InDefaultValue)
{
}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	if (const UUserSettings* UserSettings = CachedWeakUserSettings.Get())
	{
		return UserSettings->GetSetting(SettingId, DefaultValue);
	}

	return DefaultValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue) const
{
	if (UUserSettings* UserSettings = CachedWeakUserSettings.Get())
	{
		UserSettings->SetSetting(SettingId, InStringValue);
	}
}
