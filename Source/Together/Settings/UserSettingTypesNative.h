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
		default:
			return NativeSettingIds::Invalid;
	}
}

UENUM(BlueprintType)
enum class EOverallScalabilityLevel : uint8
{
	Low = 0 UMETA(DisplayName = "Low"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "High"),
	Epic = 3 UMETA(DisplayName = "Epic"),
	Cinematic = 4 UMETA(DisplayName = "Cinematic"),
};
