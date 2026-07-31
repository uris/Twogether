// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UserSettingApplyMode.h"

#include "SettingsDependancy.generated.h"

UENUM()
enum class EDependencyResult : uint8
{
	SetToValue = 0 UMETA(DisplayName = "Set an entered value"),
	SetToMatchThis = 1 UMETA(DisplayName = "Match value of dependant setting"),
	SetToMatchOther = 3 UMETA(DisplayName = "Match value of other setting"),
};

// each condition
USTRUCT(BlueprintType)
struct FSettingDependency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dependency")
	FName DependantSettingId = FName(NAME_None);

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
