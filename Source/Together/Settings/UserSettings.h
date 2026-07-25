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
	// public getter
	static UUserSettings* Get();

	//*** Gameplay Collection Tab //
	UFUNCTION()
	FString GetGameDifficulty() const;

	UFUNCTION()
	void SetGameDifficulty(const FString& InDifficulty);

private:
	UPROPERTY(Config, EditAnywhere, Category="User Settings")
	FString GameDifficulty;

};
