// Uris - All Rights Reserved

#include "NativeSettingsHelper.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettings.h"

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
	// *** Resolution values must be stored in the following format:
	// (X=123, Y=345)
	TArray<FIntPoint> SupportedResolutions;

	// get supported resolutions
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	// sort supported resolutions by X low to high
	SupportedResolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return A.X < B.X;
	});

	if (!SupportedResolutions.IsEmpty())
	{
		const FIntPoint LastRes = SupportedResolutions.Last();
		return FString::Printf(TEXT("%s x %s"), *LexToString(LastRes.X), *LexToString(LastRes.Y));
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
