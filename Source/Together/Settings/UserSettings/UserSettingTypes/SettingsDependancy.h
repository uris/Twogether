// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SettingsEnums.h"

#include "SettingsDependancy.generated.h"

// each condition
USTRUCT(BlueprintType)
struct FSettingDependency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dependency")
	FName DependantSettingId = FName(NAME_None);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Edit Condition")
	EDependencyOperator LogicalOperator = EDependencyOperator::None;

	// clang-format off
	// @formatter:off
	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "LogicalOperator != EDependencyOperator::None && LogicalOperator != EDependencyOperator::Contains",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString ComparisonStringValue = TEXT("");
	// @formatter:on
	// clang-format on

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "LogicalOperator == EDependencyOperator::Contains",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString CommaSeparatedComparisonValues = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dependency")
	EDependencyResult DependencyResult = EDependencyResult::SetToValue;

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "DependencyResult == EDependencyResult::SetToValue",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString Value = TEXT("");

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "DependencyResult == EDependencyResult::SetToMatchOther",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FName OtherSettingId = FName(NAME_None);

	UPROPERTY(
		EditAnywhere,
		Category="Behavior",
		meta = (TitleProperty = "DisplayName", ToolTip="Determines if result should save immediately"))
	bool bShouldApplyChangesImmediately = true;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Behavior",
		meta = (ToolTip= "Determines if result should apply screen resolution, etc." ))
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;
};
