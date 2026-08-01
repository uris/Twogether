// Uris - All Rights Reserved

#include "NativeSettingsHelper.h"

#include "UIFunctionLibrary.h"
#include "UserSettingTypesNative.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
template <typename EnumType>
TArray<EnumType> GetSupportedEnumValues()
{
	static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

	TArray<EnumType> Values;
	const UEnum* Enum = StaticEnum<EnumType>();
	if (!Enum)
	{
		return Values;
	}

	for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums(); ++EnumIndex)
	{
		const int64 NumericValue = Enum->GetValueByIndex(EnumIndex);
		if (NumericValue == -1)
		{
			continue;
		}

		if (EnumIndex == Enum->NumEnums() - 1 &&
		    NumericValue == Enum->GetMaxEnumValue())
		{
			continue;
		}

#if WITH_METADATA
		if (Enum->HasMetaData(TEXT("Hidden"), EnumIndex))
		{
			continue;
		}
#endif

		Values.Add(static_cast<EnumType>(NumericValue));
	}

	return Values;
}

template <typename EnumType>
bool TryParseSupportedEnumValue(const FString& InValue, EnumType& OutValue)
{
	static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

	int64 NumericValue = INDEX_NONE;
	if (!LexTryParseString(NumericValue, *InValue))
	{
		return false;
	}

	const TArray<EnumType> SupportedValues = GetSupportedEnumValues<EnumType>();
	const EnumType ParsedValue = static_cast<EnumType>(NumericValue);
	if (!SupportedValues.Contains(ParsedValue))
	{
		return false;
	}

	OutValue = ParsedValue;
	return true;
}

bool GetScalabilityPresetValues(const TCHAR* PresetKey, TArray<float>& OutPresets)
{
	OutPresets.Reset();

	TArray<FString> PresetStrings;
	if (!GConfig ||
	    !GConfig->GetSingleLineArray(
		    TEXT("ScalabilitySettings"),
		    PresetKey,
		    PresetStrings,
		    GScalabilityIni) ||
	    PresetStrings.IsEmpty())
	{
		return false;
	}

	OutPresets.Reserve(PresetStrings.Num());
	for (const FString& PresetString : PresetStrings)
	{
		float Preset = 0.0f;
		if (!LexTryParseString(Preset, *PresetString) || !FMath::IsFinite(Preset))
		{
			OutPresets.Reset();
			return false;
		}
		OutPresets.Add(Preset);
	}

	return true;
}

template <typename EnumType>
bool TryGetScalarPresetForEnum(
	const TCHAR* PresetKey,
	const EnumType InValue,
	float& OutPreset)
{
	const TArray<EnumType> SupportedValues = GetSupportedEnumValues<EnumType>();
	const int32 ValueIndex = SupportedValues.IndexOfByKey(InValue);

	TArray<float> Presets;
	if (ValueIndex == INDEX_NONE ||
	    !GetScalabilityPresetValues(PresetKey, Presets) ||
	    !Presets.IsValidIndex(ValueIndex))
	{
		return false;
	}

	OutPreset = Presets[ValueIndex];
	return true;
}

template <typename EnumType>
bool TryResolveEnumForScalarPreset(
	const TCHAR* PresetKey,
	const float CurrentValue,
	EnumType& OutValue,
	const TOptional<EnumType>& PreferredValue = TOptional<EnumType>())
{
	const TArray<EnumType> SupportedValues = GetSupportedEnumValues<EnumType>();
	TArray<float> Presets;
	if (!FMath::IsFinite(CurrentValue) ||
	    SupportedValues.IsEmpty() ||
	    !GetScalabilityPresetValues(PresetKey, Presets))
	{
		return false;
	}

	const int32 ComparableCount = FMath::Min(SupportedValues.Num(), Presets.Num());
	if (ComparableCount <= 0)
	{
		return false;
	}

	int32 ClosestIndex = 0;
	float ClosestDistance = FMath::Abs(CurrentValue - Presets[0]);
	for (int32 PresetIndex = 1; PresetIndex < ComparableCount; ++PresetIndex)
	{
		const float Distance = FMath::Abs(CurrentValue - Presets[PresetIndex]);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestIndex = PresetIndex;
		}
	}

	if (PreferredValue.IsSet())
	{
		const int32 PreferredIndex = SupportedValues.IndexOfByKey(PreferredValue.GetValue());
		if (PreferredIndex >= 0 &&
		    PreferredIndex < ComparableCount &&
		    FMath::IsNearlyEqual(Presets[PreferredIndex], Presets[ClosestIndex]))
		{
			ClosestIndex = PreferredIndex;
		}
	}

	OutValue = SupportedValues[ClosestIndex];
	return true;
}

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
	GammaSettings.StepSize = 0.05f;
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
		UE_LOG(LogTemp,
		       Warning,
		       TEXT("GetActiveScalabilityLevel: %s"),
		       *LexToString(Settings->GetOverallScalabilityLevel()));
		return LexToString(Settings->GetOverallScalabilityLevel());
	}
	return TEXT("1");
}

bool UNativeSettingsHelper::SetActiveScalabilityLevel(const FString& InValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	ENormalizedGraphicsQuality Quality;
	if (!TryParseSupportedEnumValue(InValue, Quality))
	{
		// Custom is a display-only entry with value -1. It must not be staged
		// in Unreal's native scalability state.
		return false;
	}

	UE_LOG(LogTemp,
	       Warning,
	       TEXT("SetOverallScalabilityLevel: InValue: %s, EnumValue: %i"),
	       *InValue,
	       static_cast<int32>(Quality));
	Settings->SetOverallScalabilityLevel(static_cast<int32>(Quality));
	return true;
}

FString UNativeSettingsHelper::Get3DResolutionScale()
{
	if (const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		float CurrentNormalized = 0.0f;
		float CurrentPercentage = 0.0f;
		float MinPercentage = 0.0f;
		float MaxPercentage = 0.0f;
		Settings->GetResolutionScaleInformationEx(
			CurrentNormalized,
			CurrentPercentage,
			MinPercentage,
			MaxPercentage);

		const int32 OverallQuality = Settings->GetOverallScalabilityLevel();
		TOptional<ENormalizedGraphicsQuality> PreferredQuality;
		if (OverallQuality >= 0)
		{
			PreferredQuality = static_cast<ENormalizedGraphicsQuality>(OverallQuality);
		}

		ENormalizedGraphicsQuality Quality;
		if (TryResolveEnumForScalarPreset(
			TEXT("PerfIndexValues_ResolutionQuality"),
			CurrentPercentage,
			Quality,
			PreferredQuality))
		{
			return LexToString(static_cast<int64>(Quality));
		}
	}

	return TEXT("0");
}

bool UNativeSettingsHelper::Set3DResolutionScale(const FString& InValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();

	if (!Settings)
	{
		return false;
	}

	ENormalizedGraphicsQuality Quality;
	if (!TryParseSupportedEnumValue(InValue, Quality))
	{
		return false;
	}

	float ResolutionPercentage = 0.0f;
	if (!TryGetScalarPresetForEnum(
		TEXT("PerfIndexValues_ResolutionQuality"),
		Quality,
		ResolutionPercentage))
	{
		return false;
	}

	Settings->SetResolutionScaleValueEx(ResolutionPercentage);

	return true;
}

FString UNativeSettingsHelper::GetGlobalIlluminationQuality()
{
	if (const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		return LexToString(static_cast<int32>(Settings->GetGlobalIlluminationQuality()));
	}
	return TEXT("0");
}

bool UNativeSettingsHelper::SetGlobalIlluminationQuality(const FString& InValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	ENormalizedGraphicsQuality Quality;
	if (!TryParseSupportedEnumValue(InValue, Quality))
	{
		return false;
	}

	Settings->SetGlobalIlluminationQuality(static_cast<int32>(Quality));
	return true;
}

FString UNativeSettingsHelper::GetShadowQuality()
{
	if (const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		return LexToString(static_cast<int32>(Settings->GetShadowQuality()));
	}
	return TEXT("0");
}

bool UNativeSettingsHelper::SetShadowQuality(const FString& InValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	ENormalizedGraphicsQuality Quality;
	if (!TryParseSupportedEnumValue(InValue, Quality))
	{
		return false;
	}

	Settings->SetShadowQuality(static_cast<int32>(Quality));
	return true;
}

FString UNativeSettingsHelper::GetAntiAliasingQuality()
{
	if (const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		return LexToString(static_cast<int32>(Settings->GetAntiAliasingQuality()));
	}
	return TEXT("0");
}

bool UNativeSettingsHelper::SetAntiAliasingQuality(const FString& InValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return false;
	}

	ENormalizedGraphicsQuality Quality;
	if (!TryParseSupportedEnumValue(InValue, Quality))
	{
		return false;
	}

	Settings->SetAntiAliasingQuality(static_cast<int32>(Quality));
	return true;
}
