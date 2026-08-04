// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "SettingsEnums.generated.h"

UENUM(BlueprintType)
enum class EUserSettingApplyMode : uint8
{
	ApplyAll = 0 UMETA(DisplayName = "Apply All Settings"),
	ApplyResolutionSettings = 1 UMETA(DisplayName = "Apply Resolution Settings"),
	ApplyNonResolutionSettings = 2 UMETA(DisplayName = "Apply Non-Resolution Settings"),
};

UENUM(BlueprintType)
enum class EUserSettingsApplyScope : uint8
{
	All = 0 UMETA(DisplayName = "All"),
	Resolution = 1 UMETA(DisplayName = "Resolution"),
	NonResolution = 2 UMETA(DisplayName = "Non-Resolution"),
};

UENUM()
enum class EDependencyResult : uint8
{
	SetToValue = 0 UMETA(DisplayName = "Set an entered value"),
	SetToMatchThis = 1 UMETA(DisplayName = "Match value of dependant setting"),
	SetToMatchOther = 3 UMETA(DisplayName = "Match value of other setting"),
};

UENUM()
enum class EDependencyOperator : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Equals = 1 UMETA(DisplayName = "Equals"),
	NotEquals = 2 UMETA(DisplayName = "Not Equals"),
	GreaterThan = 3 UMETA(DisplayName = "Greater Than"),
	LessThan = 4 UMETA(DisplayName = "Less Than"),
	Contains = 5 UMETA(DisplayName = "Contains"),
};

UENUM()
enum class EEditConditionOperator : uint8
{
	Equals = 0 UMETA(DisplayName = "Equals"),
	NotEquals = 1 UMETA(DisplayName = "Not Equals"),
	GreaterThan = 2 UMETA(DisplayName = "Greater Than"),
	LessThan = 3 UMETA(DisplayName = "Less Than"),
	Contains = 4 UMETA(DisplayName = "Contains"),
};

UENUM()
enum class EEditConditionGroupOperator : uint8
{
	MatchAll = 0 UMETA(DisplayName = "Match All"),
	MatchAny = 1 UMETA(DisplayName = "Match Any"),
};

UENUM(BlueprintType)
enum class ENativeUnrealSettings : uint8
{
	WindowMode = 0 UMETA(DisplayName = "Window Mode"),
	ScreenResolution = 1 UMETA(DisplayName = "Screen Resolution"),
	DisplayGamma = 2 UMETA(DisplayName = "Screen Gamma (Brightness)"),
	OverallScalabilityLevel = 3 UMETA(DisplayName = "Overall Quality"),
	ResolutionScaleNormalized = 4 UMETA(DisplayName = "3D Resolution Scale"),
	GlobalIlluminationQuality = 5 UMETA(DisplayName = "Global Illumination Quality"),
	ShadowQuality = 6 UMETA(DisplayName = "Shadow Quality"),
	AntiAliasingQuality = 7 UMETA(DisplayName = "AntiAliasing Quality"),
	ViewDistanceQuality = 8 UMETA(DisplayName = "View Distance Quality"),
	TextureQuality = 9 UMETA(DisplayName = "Texture Quality"),
	VisualEffectsQuality = 10 UMETA(DisplayName = "Visual Effects Quality"),
	PostProcessingQuality = 11 UMETA(DisplayName = "Post Processing Quality"),
	ReflectionQuality = 12 UMETA(DisplayName = "Reflection Quality"),
	FrameRateLimit = 13 UMETA(DisplayName = "Frame Rate Limit"),
	VerticalSync = 14 UMETA(DisplayName = "Vertical Sync"),
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
enum class EIntEnumType : uint8
{
	EnteredValueList = 0 UMETA(DisplayName = "Entered Value List"),
	WindowMode = 1 UMETA(DisplayName = "WindowMode"),
};

UENUM(BlueprintType)
enum class ENormalizedGraphicsQuality : uint8
{
	Low = 0 UMETA(DisplayName = "Low"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "High"),
	Epic = 3 UMETA(DisplayName = "Epic"),
	Cinematic = 4 UMETA(DisplayName = "Cinematic"),
};

UENUM(BlueprintType)
enum class EViewDistanceQuality : uint8
{
	Low = 0 UMETA(DisplayName = "Near"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "Far"),
	Epic = 3 UMETA(DisplayName = "Very Far"),
	Cinematic = 4 UMETA(DisplayName = "Cinematic"),
};

UENUM(BlueprintType)
enum class ENormalizedWindowMode : uint8
{
	Fullscreen = 0 UMETA(DisplayName = "Fullscreen"),
	WindowedFullscreen = 1 UMETA(DisplayName = "Fullscreen Window"),
	Windowed = 2 UMETA(DisplayName = "Window"),
};
