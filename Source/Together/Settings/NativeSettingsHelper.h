// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "NativeSettingsHelper.generated.h"

struct FStringSetting;
/**
 *
 */
UCLASS()
class TOGETHER_API UNativeSettingsHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// *** SCREEN RES *** //

	// retrieve a settings object with all currently supported screen resolution values
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static TArray<FStringSetting> GetSupportedResolutionsSettings(const FName& InSettingDataId);

	// get the max res setting
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString MaxSupportedResolutionString();

	// retrieve the current resolution setting in the format it is stored in settings: (X=123, Y=456)
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetCurrentResolutionString();

	// stage a screen resolution value; applying/saving is handled by the setting definition flags
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetScreenResolution(const FString& InValue);

	// *** WINDOW MODE *** //

	// retrieve the current window mode value as a string in the format it is saved in settings: 1
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetCurrentWindowMode();

	// retrieve the current window mode value as a string in the format it is saved in settings: 1
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetWindowMode(const FString& InValue);
};
