// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Settings/UserSettingTypes.h"

namespace NativeSettingIds
{
inline const FName Invalid(TEXT("Invalid"));
inline const FName WindowMode(TEXT("WindowMode"));
}

inline FName GetNativeSettingId(const ENativeUnrealSettings NativeSetting)
{
	switch (NativeSetting)
	{
		case ENativeUnrealSettings::WindowMode:
			return NativeSettingIds::WindowMode;
		default:
			return NativeSettingIds::Invalid;
	}
}
