// Uris - All Rights Reserved

#include "NativeSettingsHelper.h"

#include "UIFunctionLibrary.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettings.h"
#include "GameFramework/GameUserSettings.h"

namespace
{
bool GetMaxSupportedResolution(FIntPoint& OutResolution)
{
	TArray<FIntPoint> SupportedResolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	if (SupportedResolutions.IsEmpty())
	{
		return false;
	}

	SupportedResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return static_cast<int64>(A.X) * A.Y < static_cast<int64>(B.X) * B.Y;
	});

	OutResolution = SupportedResolutions.Last();
	return true;
}
}

TArray<FStringSetting> UNativeSettingsHelper::GetSupportedResolutionsSettings(const FName& InSettingDataId)
{
	// *** Resolution values must be stored in the following format:
	// (X=123, Y=345)
	TArray<FIntPoint> SupportedResolutions;
	TArray<FStringSetting> SupportedResolutionSettings;

	// get supported resolutions
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	// sort supported resolutions by X low to high
	SupportedResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return A.X < B.X;
	});

	// create the settings array
	for (const FIntPoint& Resolution : SupportedResolutions)
	{
		FStringSetting ResolutionSetting;

		// display values in the format 1024 x 768
		ResolutionSetting.DisplayName = FText::FromString(
			FString::Printf(TEXT("%s x %s"), *LexToString(Resolution.X), *LexToString(Resolution.Y)));

		// values stored must be in the format: (X=123, Y=345)
		ResolutionSetting.Value = FString::Printf(TEXT("(X=%s, Y=%s)"),
		                                          *LexToString(Resolution.X),
		                                          *LexToString(Resolution.Y));

		ResolutionSetting.SettingDataId = InSettingDataId;
		SupportedResolutionSettings.Add(ResolutionSetting);
	}

	// push it out
	return SupportedResolutionSettings;
}

FString UNativeSettingsHelper::MaxSupportedResolutionString()
{
	FIntPoint MaxResolution;
	if (GetMaxSupportedResolution(MaxResolution))
	{
		return FString::Printf(TEXT("%s x %s"),
		                       *LexToString(MaxResolution.X),
		                       *LexToString(MaxResolution.Y));
	}

	return FString();
}

FString UNativeSettingsHelper::GetCurrentResolutionString()
{
	if (const UUserSettings* UserSettings = UUserSettings::Get())
	{
		const FIntPoint& Resolution = UserSettings->GetScreenResolution();
		return FString::Printf(TEXT("(X=%i, Y=%i)"), Resolution.X, Resolution.Y);
	}

	return FString();
}

bool UNativeSettingsHelper::SetScreenResolution(const FString& InValue)
{
	FIntPoint Resolution = FIntPoint::ZeroValue;

	if (!Resolution.InitFromString(InValue))
	{
		return false;
	}

	if (Resolution.X <= 0 || Resolution.Y <= 0)
	{
		return false;
	}

	if (UUserSettings* UserSettings = UUserSettings::Get())
	{
		if (UserSettings->GetScreenResolution() != Resolution)
		{
			UserSettings->SetScreenResolution(Resolution);
			return true;
		}
	}

	return false;
}

void UNativeSettingsHelper::ApplyResolutionSettings(const bool bCheckForCommandLineOverrides)
{
	UUserSettings* UserSettings = UUserSettings::Get();
	if (!UserSettings)
	{
		return;
	}

	const FIntPoint StagedResolution = UserSettings->GetScreenResolution();
	const EWindowMode::Type WindowMode = UserSettings->GetFullscreenMode();
	const bool bUseMaxResolution =
		WindowMode == EWindowMode::Fullscreen ||
		WindowMode == EWindowMode::WindowedFullscreen;

	FIntPoint MaxResolution;
	if (bUseMaxResolution && GetMaxSupportedResolution(MaxResolution))
	{
		UserSettings->SetScreenResolution(MaxResolution);
	}

	UserSettings->ApplyResolutionSettings(bCheckForCommandLineOverrides);

	if (bUseMaxResolution)
	{
		// Restore the user's selected resolution in memory so a subsequent
		// SaveSettings call does not persist the runtime fullscreen resolution.
		UserSettings->SetScreenResolution(StagedResolution);
	}

	UserSettings->NotifyResolutionSettingsApplied();
}

FString UNativeSettingsHelper::GetCurrentWindowMode()
{
	if (const UUserSettings* USettings = UUserSettings::Get())
	{
		return LexToString(static_cast<int32>(USettings->GetFullscreenMode()));
	}

	return FString();
}

bool UNativeSettingsHelper::SetWindowMode(const FString& InValue)
{
	if (UUserSettings* USettings = UUserSettings::Get())
	{
		int32 ParsedValue = INDEX_NONE;
		if (!LexTryParseString(ParsedValue, *InValue))
		{
			return false;
		}

		const EWindowMode::Type NewMode = static_cast<EWindowMode::Type>(ParsedValue);
		if (NewMode != EWindowMode::Fullscreen &&
		    NewMode != EWindowMode::WindowedFullscreen &&
		    NewMode != EWindowMode::Windowed)
		{
			return false;
		}

		if (USettings->GetFullscreenMode() != NewMode)
		{
			USettings->SetFullscreenMode(NewMode);
			return true;
		}
	}

	return false;
}

FScalarSettingValues UNativeSettingsHelper::GetDisplayGammaSettings()
{
	FScalarSettingValues GammaSettings = FScalarSettingValues();
	GammaSettings.MinValue = 0.2f;
	GammaSettings.MaxValue = 4.2f;
	GammaSettings.NumericType = ECommonNumericType::Percentage;
	GammaSettings.MaximumFractionalDigits = 0.f;
	GammaSettings.MinimumFractionalDigits = 0.f;
	return GammaSettings;
}

FString UNativeSettingsHelper::GetActiveDisplayGamma()
{
	if (GEngine)
	{
		return LexToString(GEngine->GetDisplayGamma());
	}
	return TEXT("2.2f");
}

bool UNativeSettingsHelper::SetActiveDisplayGamma(const FString& InValue)
{
	if (GEngine)
	{

		GEngine->DisplayGamma = UUIFunctionLibrary::StringToFloat(InValue);
		return true;
	}

	return false;
}

FString UNativeSettingsHelper::GetActiveScalabilityLevel()
{
	if (const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		return LexToString(Settings->GetOverallScalabilityLevel());
	}
	return TEXT("1");
}

bool UNativeSettingsHelper::SetActiveScalabilityLevel(const FString& InValue)
{
	if (UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{

		Settings->SetOverallScalabilityLevel(UUIFunctionLibrary::StringToInt(InValue));
		return true;
	}

	return false;
}
