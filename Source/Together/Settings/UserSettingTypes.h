// Uris - All Rights Reserved

#pragma once

#include "CommonNumericTextBlock.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UserSettingTypes.generated.h"

USTRUCT(BlueprintType)
struct FStringSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SettingDataId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value;

	FStringSetting() = default;

	FStringSetting(const FName& InSettingDataId, const FText& InDisplayName, const FString& InValue)
		: SettingDataId(InSettingDataId), DisplayName(InDisplayName), Value(InValue) {}
};

UENUM(BlueprintType)
enum class EUserSettingValueType : uint8
{
	Bool = 0 UMETA(DisplayName = "Bool"),
	Integer = 1 UMETA(DisplayName = "Integer"),
	Scalar = 2 UMETA(DisplayName = "Scalar"),
	Float = 3 UMETA(DisplayName = "Float"),
	String = 4 UMETA(DisplayName = "String"),
};

UENUM(BlueprintType)
enum class EUserSettingTab : uint8
{
	Gameplay = 0 UMETA(DisplayName = "Gameplay"),
	Audio = 1 UMETA(DisplayName = "Audio"),
	Video = 2 UMETA(DisplayName = "Video"),
	Input = 3 UMETA(DisplayName = "Input"),
};

USTRUCT(BlueprintType)
struct FUserSettingTabDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EUserSettingTab Tab = EUserSettingTab::Gameplay;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	int32 SortOrder = 100;
};

USTRUCT(BlueprintType)
struct FUserSettingDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName SettingId;

	UPROPERTY(EditAnywhere)
	EUserSettingTab SettingTab;

	UPROPERTY(EditAnywhere)
	FName ParentSettingId;

	UPROPERTY(EditAnywhere)
	bool bIsSettingGroup = false;

	UPROPERTY(EditAnywhere)
	int32 SortOrder = 100;

	UPROPERTY(EditAnywhere)
	FText DisplayName;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FText DisabledText;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FText TechText;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	TSoftObjectPtr<UTexture2D> DescriptionImage;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	EUserSettingValueType Type = EUserSettingValueType::Float;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	bool bShouldApplyChangesImmediately = false;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString DefaultValue;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::String",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	TArray<FStringSetting> AvailableValues;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	float MinValue = 0.0f;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	float MaxValue = 1.0f;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	float StepSize = 0.01f;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides))
	ECommonNumericType NumericType = ECommonNumericType::Number;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			ClampMin = "0"))
	int32 MinimumFractionalDigits = 0;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			ClampMin = "0"))
	int32 MaximumFractionalDigits = 2;
};
