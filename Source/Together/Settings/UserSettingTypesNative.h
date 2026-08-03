// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Settings/UserSettingTypes.h"

// keep as last include
#include "UserSettingTypesNative.generated.h"

namespace NativeSettingIds
{
inline const FName Invalid(TEXT("Invalid"));
inline const FName WindowMode(TEXT("WindowMode"));
inline const FName ScreenResolution(TEXT("ScreenResolution"));
inline const FName DisplayGamma(TEXT("DisplayGamma"));
inline const FName OverallScalabilityLevel(TEXT("OverallScalabilityLevel"));
inline const FName ResolutionScaleNormalized(TEXT("ResolutionScaleNormalized"));
inline const FName GlobalIlluminationQuality(TEXT("GlobalIlluminationQuality"));
inline const FName ShadowQuality(TEXT("ShadowQuality"));
inline const FName AntiAliasingQuality(TEXT("AntiAliasingQuality"));
inline const FName ViewDistanceQuality(TEXT("ViewDistanceQuality"));
inline const FName TextureQuality(TEXT("TextureQuality"));
inline const FName VisualEffectsQuality(TEXT("VisualEffectsQuality"));
inline const FName PostProcessingQuality(TEXT("PostProcessingQuality"));
inline const FName ReflectionQuality(TEXT("ReflectionQuality"));
inline const FName FrameRateLimit(TEXT("FrameRateLimit"));
inline const FName VerticalSync(TEXT("VerticalSync"));
}

inline FName GetNativeSettingId(const ENativeUnrealSettings NativeSetting)
{
	switch (NativeSetting)
	{
		case ENativeUnrealSettings::WindowMode:
			return NativeSettingIds::WindowMode;
		case ENativeUnrealSettings::ScreenResolution:
			return NativeSettingIds::ScreenResolution;
		case ENativeUnrealSettings::DisplayGamma:
			return NativeSettingIds::DisplayGamma;
		case ENativeUnrealSettings::OverallScalabilityLevel:
			return NativeSettingIds::OverallScalabilityLevel;
		case ENativeUnrealSettings::ResolutionScaleNormalized:
			return NativeSettingIds::ResolutionScaleNormalized;
		case ENativeUnrealSettings::GlobalIlluminationQuality:
			return NativeSettingIds::GlobalIlluminationQuality;
		case ENativeUnrealSettings::ShadowQuality:
			return NativeSettingIds::ShadowQuality;
		case ENativeUnrealSettings::AntiAliasingQuality:
			return NativeSettingIds::AntiAliasingQuality;
		case ENativeUnrealSettings::TextureQuality:
			return NativeSettingIds::TextureQuality;
		case ENativeUnrealSettings::VisualEffectsQuality:
			return NativeSettingIds::VisualEffectsQuality;
		case ENativeUnrealSettings::PostProcessingQuality:
			return NativeSettingIds::PostProcessingQuality;
		case ENativeUnrealSettings::ReflectionQuality:
			return NativeSettingIds::ReflectionQuality;
		case ENativeUnrealSettings::FrameRateLimit:
			return NativeSettingIds::FrameRateLimit;
		case ENativeUnrealSettings::VerticalSync:
			return NativeSettingIds::VerticalSync;
		default:
			return NativeSettingIds::Invalid;
	}
}

UENUM(BlueprintType)
enum class ENormalizedGraphicsQuality : uint8
{
	Low = 0 UMETA(DisplayName = "Low"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "High"),
	Epic = 3 UMETA(DisplayName = "Epic"),
	Cinematic = 4 UMETA(DisplayName = "Cinematic"),
};

UENUM(BlueprintType)
enum class EViewDistanceQuality : uint8
{
	Low = 0 UMETA(DisplayName = "Near"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "Far"),
	Epic = 3 UMETA(DisplayName = "Very Far"),
	Cinematic = 4 UMETA(DisplayName = "Cinematic"),
};

UENUM(BlueprintType)
enum class ENormalizedWindowMode : uint8
{
	Fullscreen = 0 UMETA(DisplayName = "Fullscreen"),
	WindowedFullscreen = 1 UMETA(DisplayName = "Fullscreen Window"),
	Windowed = 2 UMETA(DisplayName = "Window"),
};
