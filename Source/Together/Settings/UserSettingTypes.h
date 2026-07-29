// Uris - All Rights Reserved

#pragma once

#include "CommonNumericTextBlock.h"
#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "UserSettingTypes.generated.h"

UENUM(BlueprintType)
enum class ENativeUnrealSettings : uint8
{
	WindowMode = 0 UMETA(DisplayName = "Window Mode"),
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsNativeSetting = false;

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "bIsNativeSetting == true",
			EditConditionHides,
			TitleProperty = "DisplayName"))
	ENativeUnrealSettings NativeSetting;

	UPROPERTY(EditAnywhere,
		meta = (EditCondition = "bIsNativeSetting == false",
			EditConditionHides,
			TitleProperty = "DisplayName"))
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bApplyVideoSettings = false;

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
};
