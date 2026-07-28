// Uris - All Rights Reserved


#include "UserSettings.h"

#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"

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
		if (!Definition ||
		    Definition->SettingId.IsNone() ||
		    Definition->bIsSettingGroup)
		{
			continue;
		}

		if (DefinitionsById.Contains(Definition->SettingId))
		{
			UE_LOG(LogTemp,
			       Warning,
			       TEXT("Setting IDs must be unique - %s already exists"),
			       *Definition->SettingId.ToString());
			continue;
		}

		DefinitionsById.Add(Definition->SettingId, *Definition);
		DynamicSettings.FindOrAdd(Definition->SettingId, Definition->DefaultValue);
	}
}

FString UUserSettings::GetSetting(const FName InSettingId, const FString& InFallback) const
{
	if (const FString* Value = DynamicSettings.Find(InSettingId))
	{
		return *Value;
	}

	return InFallback;
}

void UUserSettings::SetSetting(const FName InSettingId, const FString& InValue)
{
	// can't update without the setting id
	if (InSettingId.IsNone())
	{
		return;
	}

	// if values are the same, no need to update
	if (DynamicSettings.FindOrAdd(InSettingId).Equals(InValue, ESearchCase::CaseSensitive))
	{
		return;
	}

	// update and broadcast
	DynamicSettings.FindOrAdd(InSettingId) = InValue;
	if (const FUserSettingDefinition* Definition = DefinitionsById.Find(InSettingId))
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
