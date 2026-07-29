// Uris - All Rights Reserved


#include "UserSettings.h"

#include "UserSettingTypesNative.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"

#include "NativeSettingsHelper.h"

void UUserSettings::LoadSettings(const bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	InitializeDynamicSettings();
}

void UUserSettings::InitializeDynamicSettings()
{
	const UTogetherSettings* ProjectSettings = GetDefault<UTogetherSettings>();
	if (!ProjectSettings || ProjectSettings->GameSettings.IsNull())
	{
		UE_LOG(LogTemp,
		       Warning,
		       TEXT("Dynamic user settings were not initialized: no Game Settings data table is configured."));
		return;
	}

	const UDataTable* SettingsTable = ProjectSettings->GameSettings.LoadSynchronous();
	if (!SettingsTable)
	{
		UE_LOG(LogTemp,
		       Error,
		       TEXT("Dynamic user settings were not initialized: the Game Settings data table could not be loaded."));
		return;
	}

	if (SettingsTable->GetRowStruct() != FUserSettingDefinition::StaticStruct())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Dynamic user settings were not initialized: '%s' does not use FUserSettingDefinition."),
			*SettingsTable->GetName());
		return;
	}

	TArray<const FUserSettingDefinition*> Definitions;
	SettingsTable->GetAllRows(TEXT("UUserSettings::InitializeDynamicSettings"), Definitions);
	DefinitionsById.Reset();

	for (const FUserSettingDefinition* Definition : Definitions)
	{
		if (!Definition || Definition->bIsSettingGroup)
		{
			continue;
		}

		const FName EffectiveSettingId = Definition->bIsNativeSetting
			                                 ? GetNativeSettingId(Definition->NativeSetting)
			                                 : Definition->SettingId;
		if (EffectiveSettingId.IsNone() || EffectiveSettingId == NativeSettingIds::Invalid)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Skipping setting with an invalid effective setting ID."));
			continue;
		}

		if (DefinitionsById.Contains(EffectiveSettingId))
		{
			UE_LOG(LogTemp,
			       Warning,
			       TEXT("Setting IDs must be unique - %s already exists"),
			       *EffectiveSettingId.ToString());
			continue;
		}

		FUserSettingDefinition RuntimeDefinition = *Definition;
		RuntimeDefinition.SettingId = EffectiveSettingId;
		DefinitionsById.Add(EffectiveSettingId, MoveTemp(RuntimeDefinition));

		if (Definition->bIsNativeSetting)
		{
			// Remove values left behind if this setting used to be stored dynamically.
			DynamicSettings.Remove(EffectiveSettingId);
		}
		else
		{
			DynamicSettings.FindOrAdd(EffectiveSettingId, Definition->DefaultValue);
		}
	}
}

FString UUserSettings::GetSetting(const FName InSettingId, const FString& InFallback, const bool bIsNativeSetting) const
{
	FString Value;

	if (bIsNativeSetting)
	{
		Value = GetNativeSettingValue(InSettingId);
	}
	else
	{
		if (const FString* DynamicValue = DynamicSettings.Find(InSettingId))
		{
			Value = *DynamicValue;
		}
	}

	if (!Value.IsEmpty())
	{
		return Value;
	}

	return InFallback;
}

void UUserSettings::SetSetting(const FName InSettingId, const FString& InValue, const bool bIsNativeSetting)
{
	bool bDidUpdateSetting = false;

	// can't update without the setting id
	if (InSettingId.IsNone())
	{
		return;
	}

	// set a native unreal setting
	if (bIsNativeSetting)
	{
		bDidUpdateSetting = SetNativeSettingValue(InSettingId, InValue);
	}
	else
	{
		// if values are the same, no need to broadcast update
		if (DynamicSettings.FindOrAdd(InSettingId).Equals(InValue, ESearchCase::CaseSensitive))
		{
			return;
		}

		DynamicSettings.FindOrAdd(InSettingId) = InValue;
		bDidUpdateSetting = true;
	}

	// broadcast the update if the setting was updated
	if (const FUserSettingDefinition* Definition = DefinitionsById.Find(InSettingId); Definition && bDidUpdateSetting)
	{
		OnUserSettingChanged.Broadcast(*Definition, InValue);
	}
}

UUserSettings* UUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

FString UUserSettings::GetNativeSettingValue(const FName InSettingId)
{

	if (InSettingId == NativeSettingIds::WindowMode)
	{
		return UNativeSettingsHelper::GetCurrentWindowMode();
	}

	if (InSettingId == NativeSettingIds::ScreenResolution)
	{
		return UNativeSettingsHelper::GetCurrentResolutionString();
	}

	return FString();
}

bool UUserSettings::SetNativeSettingValue(const FName InSettingId, const FString& InValue)
{
	if (InSettingId == NativeSettingIds::WindowMode)
	{
		return UNativeSettingsHelper::SetWindowMode(InValue);
	}

	if (InSettingId == NativeSettingIds::ScreenResolution)
	{
		return UNativeSettingsHelper::SetScreenResolution(InValue);
	}

	// *** Fallback *** //
	return false;
}
