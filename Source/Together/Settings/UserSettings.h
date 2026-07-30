// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Settings/UserSettingTypes.h"
#include "UserSettings.generated.h"

UENUM(BlueprintType)
enum class EUserSettingsApplyScope : uint8
{
	All = 0 UMETA(DisplayName = "All"),
	Resolution = 1 UMETA(DisplayName = "Resolution"),
	NonResolution = 2 UMETA(DisplayName = "Non-Resolution"),
};

USTRUCT(BlueprintType)
struct FUserSettingChange
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="User Settings")
	FName SettingId;

	UPROPERTY(BlueprintReadOnly, Category="User Settings")
	FString PreviousValue;

	UPROPERTY(BlueprintReadOnly, Category="User Settings")
	FString NewValue;

	FUserSettingChange() = default;

	FUserSettingChange(const FName InSettingId, const FString& InPreviousValue, const FString& InNewValue)
		: SettingId(InSettingId),
		  PreviousValue(InPreviousValue),
		  NewValue(InNewValue) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserSettingValueChanged,
                                            const FUserSettingChange&,
                                            Change);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSettingsApplied,
                                             const TArray<FUserSettingChange>&,
                                             Changes,
                                             EUserSettingsApplyScope,
                                             Scope);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsSaved,
                                            const TArray<FName>&,
                                            SettingIds);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsLoaded);

/**
 *
 */
UCLASS()
class TOGETHER_API UUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	/** The configured in-memory value changed. It may not yet be applied or saved. */
	UPROPERTY(BlueprintAssignable, Category="User Settings")
	FOnUserSettingValueChanged OnSettingChanged;

	/** The listed setting values now affect the running game. */
	UPROPERTY(BlueprintAssignable, Category="User Settings")
	FOnSettingsApplied OnSettingsApplied;

	/** The listed setting values were persisted. */
	UPROPERTY(BlueprintAssignable, Category="User Settings")
	FOnSettingsSaved OnSettingsSaved;

	/** Persisted settings finished loading and are available to query. */
	UPROPERTY(BlueprintAssignable, Category="User Settings")
	FOnSettingsLoaded OnSettingsLoaded;

	// initial loading of data table with user settings
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual void ApplyNonResolutionSettings() override;
	virtual void SaveSettings() override;

	// *** generic string-based access for data-driven settings *** //
	FString GetSetting(const FName InSettingId, const FString& InFallback, const bool bIsNativeSetting = false) const;
	void SetSetting(FName InSettingId, const FString& InValue, bool bIsNativeSetting = false);

	/** Called by the native settings helper after Unreal applies staged resolution settings. */
	void NotifyResolutionSettingsApplied();

	// helper to get user settings object
	static UUserSettings* Get();

private:
	// init helper
	void InitializeDynamicSettings();

	// private getters/setter for native settings
	static FString GetNativeSettingValue(FName InSettingId);
	static bool SetNativeSettingValue(const FName InSettingId, const FString& InValue);

	void TrackSettingChange(FName InSettingId, const FString& InPreviousValue, const FString& InNewValue);
	void BroadcastAppliedSettings(EUserSettingsApplyScope InScope);
	bool DoesSettingMatchApplyScope(FName InSettingId, EUserSettingsApplyScope InScope) const;

	// lookup reference of settings used in update broadcasts
	TMap<FName, FUserSettingDefinition> DefinitionsById;

	TMap<FName, FUserSettingChange> UnappliedChanges;
	TMap<FName, FUserSettingChange> UnsavedChanges;
	bool bApplyingAllSettings = false;

	// values saved as map of config values to settings ini
	UPROPERTY(Config)
	TMap<FName, FString> DynamicSettings;
};
