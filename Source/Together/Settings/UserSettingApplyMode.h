// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "UserSettingApplyMode.generated.h"

UENUM(BlueprintType)
enum class EUserSettingApplyMode : uint8
{
	ApplyAll = 0 UMETA(DisplayName = "Apply All Settings"),
	ApplyResolutionSettings = 1 UMETA(DisplayName = "Apply Resolution Settings"),
	ApplyNonResolutionSettings = 2 UMETA(DisplayName = "Apply Non-Resolution Settings"),
};
