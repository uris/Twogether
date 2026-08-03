// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UserSettingTypes.h"
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

	// apply the staged resolution settings; fullscreen modes use the maximum supported
	// resolution at runtime without replacing the staged resolution that will be saved
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static void ApplyResolutionSettings(bool bCheckForCommandLineOverrides = false);

	// *** WINDOW MODE *** //

	// retrieve the current window mode value as a string in the format it is saved in settings: 1
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetCurrentWindowMode();

	// retrieve the current window mode value as a string in the format it is saved in settings: 1
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetWindowMode(const FString& InValue);

	// *** GAMMA *** //
	// Helps configure brightness / contrast

	// get the gamma settings values
	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FScalarSettingValues GetDisplayGammaSettings();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetActiveDisplayGamma();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetActiveDisplayGamma(const FString& InValue);

	// *** SCALABILITY LEVEL *** //
	// Helps configure brightness / contrast

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetActiveScalabilityLevel();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetActiveScalabilityLevel(const FString& InValue);

	// *** 3D RESOLUTION SCALE NORMALIZED *** //
	/**
	 * Does not affect UI. Only 3D world space. Virtually reduces the screen resolution for 3D rendering
	 * purposes by a factor, essentially reducing the "rendering" resolution. Trades 3D rendering
	 * quality (more pixelation at low levels) for better/faster performance. Internally
	 * calculates a min/max rendering scale range for the current screen resolution, then
	 * gets/sets a normalized 0-1 value along that range to drive the rendering resolution
	 * Note: for direct access to get/set unnormalized scale as a percentage 0-1, use
	 * GetResolutionScaleInformationEx() / SetResolutionScaleValueEx(float)
	*/

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString Get3DResolutionScale();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool Set3DResolutionScale(const FString& InValue);

	// *** GLOBAL ILLUMINATION QUALITY *** //
	// Control lighting, ray tracing, etc.

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetGlobalIlluminationQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetGlobalIlluminationQuality(const FString& InValue);

	// *** SHADOW QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetShadowQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetShadowQuality(const FString& InValue);

	// *** ANTI-ALIASING QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetAntiAliasingQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetAntiAliasingQuality(const FString& InValue);

	// *** VIEW DISTANCE QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetViewDistanceQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetViewDistanceQuality(const FString& InValue);

	// *** TEXTURE QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetTextureQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetTextureQuality(const FString& InValue);

	// *** POST PROCESSING QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetPostProcessingQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetPostProcessingQuality(const FString& InValue);

	// *** REFLECTION QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetReflectionQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetReflectionQuality(const FString& InValue);

	// *** VISUAL EFFECTS QUALITY *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetVisualEffectsQuality();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetVisualEffectsQuality(const FString& InValue);

	// *** FRAMERATE LIMIT *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static TArray<FStringSetting> GetFrameRateLimitSettings();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetFrameRateLimit();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetFrameRateLimit(const FString& InValue);

	// *** VERTICAL SYNC *** //

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static TArray<FStringSetting> GetVerticalSyncSettings();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static FString GetVerticalSync();

	UFUNCTION(BlueprintCallable, Category="Game Settings|Video")
	static bool SetVerticalSync(const FString& InValue);

};
