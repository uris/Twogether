// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "UserSettings.generated.h"

/**
 *
 */
UCLASS()
class TOGETHER_API UUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	virtual void LoadSettings(bool bForceReload = false) override;

	// *** generic string-based access for data-driven settings *** //

	FString GetSetting(FName InSettingId, const FString& InFallback = FString()) const;

	void SetSetting(FName InSettingId, const FString& InValue);


	// public getter
	static UUserSettings* Get();

private:
	void InitializeDynamicSettings();

	UPROPERTY(Config)
	TMap<FName, FString> DynamicSettings;
};
