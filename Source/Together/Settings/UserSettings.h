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
	// constructor
	UUserSettings();

	// public getter
	static UUserSettings* Get();

	//*** Gameplay Collection Tab //
	UFUNCTION()
	FString GetGameDifficulty() const;

	UFUNCTION()
	void SetGameDifficulty(const FString& InDifficulty);

	//*** Audio Collection Tab //
	UFUNCTION()
	float GetOverallVolume() const;

	UFUNCTION()
	void SetOverallVolume(const float InVolume);

private:
	UPROPERTY(Config, EditAnywhere, Category="User Settings|Game")
	FString GameDifficulty;

	UPROPERTY(Config, EditAnywhere, Category="User Settings|Audio")
	float OverallVolume;
};
