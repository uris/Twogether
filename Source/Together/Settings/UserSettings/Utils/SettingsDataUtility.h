// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "../UserSettingTypes/SettingsDependancy.h"
#include "../UserSettingTypes/SettingsEditCondition.h"

class TOGETHER_API FSettingsDataUtility
{
public:
	static bool DoesValueMatch(
		EEditConditionOperator Operator,
		const FString& TargetSettingCurrentValue,
		const FString& ComparisonValue,
		const FString& CommaSeparatedComparisonValues);

	static bool DoesValueMatch(
		EDependencyOperator Operator,
		const FString& TargetSettingCurrentValue,
		const FString& ComparisonValue,
		const FString& CommaSeparatedComparisonValues);
};
