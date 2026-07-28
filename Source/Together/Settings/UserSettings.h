// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "Settings/UserSettingTypes.h"
#include "UserSettings.generated.h"

// broadcast updates to setting for custom actions based on updated settings
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUserSettingChanged,
                                             const FUserSettingDefinition&,
                                             SettingDefinition,
                                             const FString&,
                                             UpdatedValue);

/**
 *
 */
UCLASS()
class TOGETHER_API UUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	// broadcast updates to setting for custom actions based on updated settings
	UPROPERTY(BlueprintAssignable, Category="User Settings")
	FOnUserSettingChanged OnUserSettingChanged;

	// initial loading of data table with user settings
	virtual void LoadSettings(bool bForceReload = false) override;

	// *** generic string-based access for data-driven settings *** //
	FString GetSetting(FName InSettingId, const FString& InFallback = FString()) const;
	void SetSetting(FName InSettingId, const FString& InValue);

	// helper to get user settings object
	static UUserSettings* Get();

private:
	// init helper
	void InitializeDynamicSettings();

	// lookup reference of settings used in update broadcasts
	TMap<FName, FUserSettingDefinition> DefinitionsById;

	// values saved as map of config values to settings ini
	UPROPERTY(Config)
	TMap<FName, FString> DynamicSettings;
};
