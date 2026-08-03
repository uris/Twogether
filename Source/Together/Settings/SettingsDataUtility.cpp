// Uris - All Rights Reserved

#include "SettingsDataUtility.h"

namespace
{
enum class ESettingsComparisonOperator : uint8
{
	None,
	Equals,
	NotEquals,
	GreaterThan,
	LessThan,
	Contains,
};

bool DoesValueMatch(
	const ESettingsComparisonOperator Operator,
	const FString& TargetSettingCurrentValue,
	const FString& ComparisonValue,
	const FString& CommaSeparatedComparisonValues)
{
	switch (Operator)
	{
		case ESettingsComparisonOperator::None:
			return true;

		case ESettingsComparisonOperator::Equals:
			return TargetSettingCurrentValue.Equals(ComparisonValue, ESearchCase::CaseSensitive);

		case ESettingsComparisonOperator::NotEquals:
			return !TargetSettingCurrentValue.Equals(ComparisonValue, ESearchCase::CaseSensitive);

		case ESettingsComparisonOperator::GreaterThan:
		case ESettingsComparisonOperator::LessThan:
		{
			double CurrentNumber = 0.0;
			double ComparisonNumber = 0.0;
			if (!LexTryParseString(CurrentNumber, *TargetSettingCurrentValue) ||
			    !LexTryParseString(ComparisonNumber, *ComparisonValue))
			{
				return false;
			}

			return Operator == ESettingsComparisonOperator::GreaterThan
				       ? CurrentNumber > ComparisonNumber
				       : CurrentNumber < ComparisonNumber;
		}

		case ESettingsComparisonOperator::Contains:
		{
			TArray<FString> ComparisonValues;
			CommaSeparatedComparisonValues.ParseIntoArray(ComparisonValues, TEXT(","), true);

			for (FString& CandidateValue : ComparisonValues)
			{
				CandidateValue.TrimStartAndEndInline();
			}

			return ComparisonValues.Contains(TargetSettingCurrentValue);
		}
	}

	return false;
}
}

bool FSettingsDataUtility::DoesValueMatch(
	const EEditConditionOperator Operator,
	const FString& TargetSettingCurrentValue,
	const FString& ComparisonValue,
	const FString& CommaSeparatedComparisonValues)
{
	ESettingsComparisonOperator ComparisonOperator;
	switch (Operator)
	{
		case EEditConditionOperator::Equals:
			ComparisonOperator = ESettingsComparisonOperator::Equals;
			break;
		case EEditConditionOperator::NotEquals:
			ComparisonOperator = ESettingsComparisonOperator::NotEquals;
			break;
		case EEditConditionOperator::GreaterThan:
			ComparisonOperator = ESettingsComparisonOperator::GreaterThan;
			break;
		case EEditConditionOperator::LessThan:
			ComparisonOperator = ESettingsComparisonOperator::LessThan;
			break;
		case EEditConditionOperator::Contains:
			ComparisonOperator = ESettingsComparisonOperator::Contains;
			break;
		default:
			return false;
	}

	return ::DoesValueMatch(
		ComparisonOperator,
		TargetSettingCurrentValue,
		ComparisonValue,
		CommaSeparatedComparisonValues);
}

bool FSettingsDataUtility::DoesValueMatch(
	const EDependencyOperator Operator,
	const FString& TargetSettingCurrentValue,
	const FString& ComparisonValue,
	const FString& CommaSeparatedComparisonValues)
{
	ESettingsComparisonOperator ComparisonOperator;
	switch (Operator)
	{
		case EDependencyOperator::None:
			ComparisonOperator = ESettingsComparisonOperator::None;
			break;
		case EDependencyOperator::Equals:
			ComparisonOperator = ESettingsComparisonOperator::Equals;
			break;
		case EDependencyOperator::NotEquals:
			ComparisonOperator = ESettingsComparisonOperator::NotEquals;
			break;
		case EDependencyOperator::GreaterThan:
			ComparisonOperator = ESettingsComparisonOperator::GreaterThan;
			break;
		case EDependencyOperator::LessThan:
			ComparisonOperator = ESettingsComparisonOperator::LessThan;
			break;
		case EDependencyOperator::Contains:
			ComparisonOperator = ESettingsComparisonOperator::Contains;
			break;
		default:
			return false;
	}

	return ::DoesValueMatch(
		ComparisonOperator,
		TargetSettingCurrentValue,
		ComparisonValue,
		CommaSeparatedComparisonValues);
}
