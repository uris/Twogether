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
	String = 0 UMETA(DisplayName = "String (single word value)"),
	Bool = 1 UMETA(DisplayName = "Bool (on/off, true/false, etc.)"),
	Scalar = 2 UMETA(DisplayName = "Scalar (floats, ints, with a range)"),
	Enum = 3 UMETA(DisplayName = "Enum (structured string lists values)"),
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
struct FScalarSettingValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StepSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECommonNumericType NumericType = ECommonNumericType::Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinimumFractionalDigits = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaximumFractionalDigits = 2;

	FScalarSettingValues() = default;
};

USTRUCT(BlueprintType)
struct FStringSettingValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString StringValue;

	FStringSettingValue() = default;

	FStringSettingValue(const FText& InDisplayName, const FString& InStringValue)
		: DisplayName(InDisplayName), StringValue(InStringValue) {}
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

	UPROPERTY(EditAnywhere,
		Category="Definition",
		BlueprintReadWrite,
		meta=(ToolTip="Must be a unique setting identifier"))
	FName SettingId;

	UPROPERTY(EditAnywhere,
		Category="Definition",
		meta=(ToolTip="Setting groups create visual heirarchy and are functionaly inert"))
	bool bIsSettingGroup = false;

	UPROPERTY(
		EditAnywhere,
		Category="Definition",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip=
			"Native settings hook into Unreal native capabilities and values, eg. resolution, window mode, etc."))
	bool bIsNativeSetting = false;

	UPROPERTY(
		EditAnywhere,
		Category="Definition",
		meta = (EditCondition = "bIsNativeSetting == true",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Select from supported native settings"))
	ENativeUnrealSettings NativeSetting;

	UPROPERTY(
		EditAnywhere,
		Category="Definition",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip=
			"Drives how the setting is displayed and interacted with (slider, rotator, etc.)"))
	EUserSettingValueType Type = EUserSettingValueType::String;

	UPROPERTY(EditAnywhere, Category="Setting Group", meta=(ToolTip="The high level tab the setting belongs under"))
	EUserSettingTab SettingTab;

	UPROPERTY(EditAnywhere,
		Category="Setting Group",
		meta=(ToolTip="Nest this setting under another setting visually (group or individual item)"))
	FName ParentSettingId;

	UPROPERTY(EditAnywhere,
		Category="Display",
		meta=(ToolTip="The order in which the setting should appear in the UI"))
	int32 SortOrder = 100;

	UPROPERTY(EditAnywhere,
		Category="Display",
		meta=(ToolTip="The name for the setting displayed as a title in the UI"))
	FText DisplayName;

	UPROPERTY(
		EditAnywhere,
		Category="Display",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	FText Description;

	UPROPERTY(
		EditAnywhere,
		Category="Display",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Populates the details pane. Leave empty to not omit it"))
	TSoftObjectPtr<UTexture2D> DescriptionImage;

	UPROPERTY(
		EditAnywhere,
		Category="Behavior",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName", ToolTip="Determines if updated settings should save immediately"))
	bool bShouldApplyChangesImmediately = true;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Behavior",
		meta = (ToolTip=
			"Determines if settings should apply, enabling deferring applying settings that affect screen resolution, etc."
		))
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;

	/*
	 * Default should be the string equivalent of the value type
	 * e.g.: if setting is a bool value, the default should be "true" or "false"
	 * regardless of the display name used for each value
	 */
	UPROPERTY(
		EditAnywhere,
		Category = "Values",
		meta = (EditCondition = "bIsSettingGroup == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FString DefaultValue;

	UPROPERTY(
		EditAnywhere,
		Category = "Values",
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::String",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	TArray<FStringSettingValue> AvailableStringValues;

	UPROPERTY(
		EditAnywhere,
		Category = "Values",
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Bool",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FBoolSetting AvailableBoolValues;

	UPROPERTY(
		EditAnywhere,
		Category = "Values",
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Enum",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	EIntEnumType AvailableEnumValues;

	UPROPERTY(
		EditAnywhere,
		Category = "Values",
		meta = (EditCondition = "bIsSettingGroup == false && Type == EUserSettingValueType::Scalar",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	FScalarSettingValues AvailableScalarValues;

	UPROPERTY(EditAnywhere, Category="Editing Rules")
	FSettingEditConditionDefinition EditConditions;
};
