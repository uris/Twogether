// Uris - All Rights Reserved

#pragma once

#include "CommonNumericTextBlock.h"
#include "CoreMinimal.h"
#include "SettingsEditCondition.h"
#include "Engine/DataTable.h"

#include "UserSettingTypes.generated.h"

UENUM(BlueprintType)
enum class ENativeUnrealSettings : uint8
{
	WindowMode = 0 UMETA(DisplayName = "Window Mode"),
	ScreenResolution = 1 UMETA(DisplayName = "Screen Resolution"),
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

UENUM(BlueprintType)
enum class EUserSettingApplyMode : uint8
{
	ApplyAll = 0 UMETA(DisplayName = "Apply All Settings"),
	ApplyResolutionSettings = 1 UMETA(DisplayName = "Apply Resolution Settings"),
	ApplyNonResolutionSettings = 2 UMETA(DisplayName = "Apply Non-Resolution Settings"),
};

//TODO: complete the list of video/other enums settings will support
UENUM(BlueprintType)
enum class EIntEnumType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	WindowMode = 1 UMETA(DisplayName = "WindowMode"),
	Resolution = 2 UMETA(DisplayName = "Resolution"),
};

USTRUCT(BlueprintType)
struct FIntEnumSetting
{
	GENERATED_BODY()

	// defined enum to use
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIntEnumType IntEnumType = EIntEnumType::None;

	// log enum names/values to help override
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLogEnumValuesAndNames = false;

	// apply custom display names for the different enum indexes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FName> CustomNamesOverride;

	FIntEnumSetting() = default;

	explicit FIntEnumSetting(const EIntEnumType& InIntEnumType)
		: IntEnumType(InIntEnumType) {}
};

USTRUCT(BlueprintType)
struct FBoolSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TrueDisplayName = FText::FromString(TEXT("On"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText FalseDisplayName = FText::FromString(TEXT("Off"));

	FBoolSetting() = default;

	FBoolSetting(const FText& InTrueDisplayName, const FText& InFalseDisplayName)
		: TrueDisplayName(InTrueDisplayName), FalseDisplayName(InFalseDisplayName) {}
};

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Must be a unique setting identifier"))
	FName SettingId;

	UPROPERTY(EditAnywhere, meta=(ToolTip="Setting groups create visual heirarchy and are functionaly inert"))
	bool bIsSettingGroup = false;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip=
			"Native settings hook into Unreal native capabilities and values, eg. resolution, window mode, etc."))
	bool bIsNativeSetting = false;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsNativeSetting == true",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Select from supported native settings"))
	ENativeUnrealSettings NativeSetting;

	UPROPERTY(EditAnywhere, meta=(ToolTip="The high level tab the setting belongs under"))
	EUserSettingTab SettingTab;

	UPROPERTY(EditAnywhere,
		meta=(ToolTip="Nest this setting under another setting visually (group or individual item)"))
	FName ParentSettingId;

	UPROPERTY(EditAnywhere, meta=(ToolTip="The order in which the setting should appear in the UI"))
	int32 SortOrder = 100;

	UPROPERTY(EditAnywhere, meta=(ToolTip="The name for the setting displayed as a title in the UI"))
	FText DisplayName;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	FText Description;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	FText DisabledText;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	FText TechText;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	TSoftObjectPtr<UTexture2D> DescriptionImage;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip=
			"Drives how the setting is displayed and interacted with (slider, rotator, etc.)"))
	EUserSettingValueType Type = EUserSettingValueType::Float;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Determines if updated settings should save immediately"))
	bool bShouldApplyChangesImmediately = true;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		meta = (ToolTip=
			"Determines if settings should apply, enabling deferring applying settings that affect screen resolution, etc."
		))
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;

	/*
	 * Default should be the string equivalent of the value type
	 * Eg: if setting is a bool value, the default should be "true" or "false"
	 * regardless of the display name used for each value
	 */
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
	TArray<FStringSetting> AvailableStringValues;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Bool",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FBoolSetting AvailableBoolValues;

	UPROPERTY(
		EditAnywhere,
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Integer",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	EIntEnumType AvailableEnumValues;

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

	UPROPERTY(EditAnywhere)
	FSettingEditConditionDefinition EditConditions;
};
