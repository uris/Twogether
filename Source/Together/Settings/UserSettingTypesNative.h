// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Settings/UserSettingTypes.h"

namespace NativeSettingIds
{
inline const FName Invalid(TEXT("Invalid"));
inline const FName WindowMode(TEXT("WindowMode"));
inline const FName ScreenResolution(TEXT("ScreenResolution"));
inline const FName DisplayGamma(TEXT("DisplayGamma"));
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
		default:
			return NativeSettingIds::Invalid;
	}
}
