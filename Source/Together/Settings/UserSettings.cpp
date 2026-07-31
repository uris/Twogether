// Uris - All Rights Reserved


#include "UserSettings.h"

#include "UserSettingTypesNative.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"

#include "NativeSettingsHelper.h"
#include "UIFunctionLibrary.h"

void UUserSettings::LoadSettings(const bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	InitializeDynamicSettings();
	UnappliedChanges.Reset();
	UnsavedChanges.Reset();

	OnSettingsLoaded.Broadcast();
}

void UUserSettings::ApplySettings(const bool bCheckForCommandLineOverrides)
{
	bApplyingAllSettings = true;
	Super::ApplySettings(bCheckForCommandLineOverrides);
	bApplyingAllSettings = false;

	BroadcastAppliedSettings(EUserSettingsApplyScope::All);

	TArray<FName> SavedSettingIds;
	UnsavedChanges.GenerateKeyArray(SavedSettingIds);
	if (!SavedSettingIds.IsEmpty())
	{
		UnsavedChanges.Reset();
		OnSettingsSaved.Broadcast(SavedSettingIds);
	}
}

void UUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	if (!bApplyingAllSettings)
	{
		BroadcastAppliedSettings(EUserSettingsApplyScope::NonResolution);
	}
}

void UUserSettings::SaveSettings()
{
	Super::SaveSettings();

	if (bApplyingAllSettings || UnsavedChanges.IsEmpty())
	{
		return;
	}

	TArray<FName> SavedSettingIds;
	UnsavedChanges.GenerateKeyArray(SavedSettingIds);
	UnsavedChanges.Reset();
	OnSettingsSaved.Broadcast(SavedSettingIds);
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

	const FString PreviousValue = GetSetting(InSettingId, FString(), bIsNativeSetting);

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

	if (bDidUpdateSetting)
	{
		TrackSettingChange(InSettingId, PreviousValue, InValue);
	}
}

void UUserSettings::NotifyResolutionSettingsApplied()
{
	if (!bApplyingAllSettings)
	{
		BroadcastAppliedSettings(EUserSettingsApplyScope::Resolution);
	}
}

void UUserSettings::TrackSettingChange(const FName InSettingId,
                                       const FString& InPreviousValue,
                                       const FString& InNewValue)
{
	FUserSettingChange& UnappliedChange = UnappliedChanges.FindOrAdd(
		InSettingId,
		FUserSettingChange(InSettingId, InPreviousValue, InNewValue));
	UnappliedChange.NewValue = InNewValue;
	if (UnappliedChange.PreviousValue.Equals(UnappliedChange.NewValue, ESearchCase::CaseSensitive))
	{
		UnappliedChanges.Remove(InSettingId);
	}

	FUserSettingChange& UnsavedChange = UnsavedChanges.FindOrAdd(
		InSettingId,
		FUserSettingChange(InSettingId, InPreviousValue, InNewValue));
	UnsavedChange.NewValue = InNewValue;
	if (UnsavedChange.PreviousValue.Equals(UnsavedChange.NewValue, ESearchCase::CaseSensitive))
	{
		UnsavedChanges.Remove(InSettingId);
	}

	OnSettingChanged.Broadcast(FUserSettingChange(InSettingId, InPreviousValue, InNewValue));
}

void UUserSettings::BroadcastAppliedSettings(const EUserSettingsApplyScope InScope)
{
	TArray<FUserSettingChange> AppliedChanges;
	TArray<FName> AppliedSettingIds;

	for (const TPair<FName, FUserSettingChange>& Pair : UnappliedChanges)
	{
		if (DoesSettingMatchApplyScope(Pair.Key, InScope))
		{
			AppliedChanges.Add(Pair.Value);
			AppliedSettingIds.Add(Pair.Key);
		}
	}

	if (AppliedChanges.IsEmpty())
	{
		return;
	}

	for (const FName AppliedSettingId : AppliedSettingIds)
	{
		UnappliedChanges.Remove(AppliedSettingId);
	}

	OnSettingsApplied.Broadcast(AppliedChanges, InScope);
}

bool UUserSettings::DoesSettingMatchApplyScope(const FName InSettingId,
                                               const EUserSettingsApplyScope InScope) const
{
	if (InScope == EUserSettingsApplyScope::All)
	{
		return true;
	}

	const FUserSettingDefinition* Definition = DefinitionsById.Find(InSettingId);
	if (!Definition)
	{
		return InScope == EUserSettingsApplyScope::NonResolution;
	}

	return InScope == EUserSettingsApplyScope::Resolution
		       ? Definition->ApplyMode == EUserSettingApplyMode::ApplyResolutionSettings
		       : Definition->ApplyMode == EUserSettingApplyMode::ApplyNonResolutionSettings ||
		         Definition->ApplyMode == EUserSettingApplyMode::ApplyAll;
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

	if (InSettingId == NativeSettingIds::DisplayGamma)
	{
		return UNativeSettingsHelper::GetActiveDisplayGamma();
	}

	if (InSettingId == NativeSettingIds::OverallScalabilityLevel)
	{
		return UNativeSettingsHelper::GetActiveScalabilityLevel();
	}

	if (InSettingId == NativeSettingIds::ResolutionScaleNormalized)
	{
		return UNativeSettingsHelper::Get3DResolutionScale();
	}

	if (InSettingId == NativeSettingIds::GlobalIlluminationQuality)
	{
		return UNativeSettingsHelper::GetGlobalIlluminationQuality();
	}

	if (InSettingId == NativeSettingIds::ShadowQuality)
	{
		return UNativeSettingsHelper::GetShadowQuality();
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

	if (InSettingId == NativeSettingIds::DisplayGamma)
	{
		return UNativeSettingsHelper::SetActiveDisplayGamma(InValue);
	}

	if (InSettingId == NativeSettingIds::OverallScalabilityLevel)
	{
		return UNativeSettingsHelper::SetActiveScalabilityLevel(InValue);
	}

	if (InSettingId == NativeSettingIds::ResolutionScaleNormalized)
	{
		return UNativeSettingsHelper::Set3DResolutionScale(InValue);
	}

	if (InSettingId == NativeSettingIds::GlobalIlluminationQuality)
	{
		return UNativeSettingsHelper::SetGlobalIlluminationQuality(InValue);
	}

	if (InSettingId == NativeSettingIds::ShadowQuality)
	{
		return UNativeSettingsHelper::SetShadowQuality(InValue);
	}

	// *** Fallback *** //
	return false;
}

UUserSettings* UUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}
