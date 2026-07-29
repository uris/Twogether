// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SettingsEditCondition.generated.h"

UENUM()
enum class EEditConditionOperator : uint8
{
	Equals = 0 UMETA(DisplayName = "Equals"),
	NotEquals = 1 UMETA(DisplayName = "Not Equals"),
	GreaterThan = 2 UMETA(DisplayName = "Greater Than"),
	LessThan = 3 UMETA(DisplayName = "Less Than"),
	Contains = 4 UMETA(DisplayName = "Contains"),
};

// Match All = logical AND
UENUM()
enum class EEditConditionGroupOperator : uint8
{
	MatchAll = 0 UMETA(DisplayName = "Match All"),
	MatchAny = 1 UMETA(DisplayName = "Match Any"),
};

// each condition
USTRUCT(BlueprintType)
struct FSettingEditCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edit Condition")
	FName TargetSettingDataId = FName(NAME_None);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edit Condition")
	EEditConditionOperator LogicalOperator = EEditConditionOperator::Equals;

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "LogicalOperator != EEditConditionOperator::Contains",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString ComparisonStringValue = TEXT("");

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "LogicalOperator == EEditConditionOperator::Contains",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString CommaSeparatedComparisonValues;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		meta=(ToolTip="Rich text message displayed on the details pane when editing condition is not met"))
	FString EditingDisabledMessage = TEXT("");
};

// definition can include multiple conditions
USTRUCT(BlueprintType)
struct FSettingEditConditionDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edit Condition")
	TArray<FSettingEditCondition> Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edit Condition")
	EEditConditionGroupOperator MatchType = EEditConditionGroupOperator::MatchAny;
};
