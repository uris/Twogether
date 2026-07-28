// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Engine/DeveloperSettings.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "TogetherSettings.generated.h"

class UUISoundFXs;
class UUIIconSet;

/**
 *
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Developer Settings"))
class TOGETHER_API UTogetherSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget References", meta = (ForceInlineRow, Categories = "UI.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> UIWidgetMap;

	UPROPERTY(Config, EditAnywhere, Category="UI|Icons")
	TSoftObjectPtr<UUIIconSet> DefaultIconSet;

	UPROPERTY(Config, EditAnywhere, Category="UI|SoundFX")
	TSoftObjectPtr<UUISoundFXs> DefaultSoundFX;

	UPROPERTY(Config, EditAnywhere, Category="UI|Images")
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> UIImages;

	UPROPERTY(Config, EditAnywhere, Category = "Global Debugging", meta = (DisplayName = "Hide Debug Messages"))
	bool bGlobalHideDebugMessages = true;

	UPROPERTY(Config,
		EditAnywhere,
		Category = "Global Debugging | List Sessions",
		meta = (DisplayName = "List Fake Game Sessions"))
	bool bListFakeGameSessions = false;

	UPROPERTY(Config,
		EditAnywhere,
		Category = "Game Data",
		meta = (DisplayName = "Data Tables"))
	TMap<FName, TSoftObjectPtr<UDataTable>> GameData;

	UPROPERTY(Config,
		EditAnywhere,
		Category = "Game Settings|Settings")
	TSoftObjectPtr<UDataTable> GameSettings;

	UPROPERTY(Config,
		EditAnywhere,
		Category = "Game Settings|Tabs")
	TSoftObjectPtr<UDataTable> GameSettingTabs;

	UPROPERTY(Config, EditAnywhere, Category = "Maps|On Server Create")
	TSoftObjectPtr<UWorld> OnServerCreateMap;

};
