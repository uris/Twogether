#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "Engine/DataTable.h"
#include "SharedTypes.generated.h"

/**
 * ENUMS
 */
UENUM(BlueprintType)
enum class EConfirmationScreenType : uint8
{
	Ok UMETA(DisplayName = "Ok Only"),
	YesNo UMETA(DisplayName = "Yes / No"),
	OkCancel UMETA(DisplayName = "Ok and Cancel"),
	Unknown UMETA(Hidden, DisplayName = "Hidden"),
};

UENUM(BlueprintType)
enum class EConfirmationButtonType : uint8
{
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),
	Close UMETA(DisplayName = "Close"),
	Unknown UMETA(Hidden, DisplayName = "Hidden"),
};

UENUM(BlueprintType)
enum class EConfirmationButtonLabel : uint8
{
	Yes UMETA(DisplayName = "Yes"),
	No UMETA(DisplayName = "No"),
	Cancel UMETA(DisplayName = "Cancel"),
	Ok UMETA(DisplayName = "Ok"),
};

UENUM()
enum class EInputValidationType : uint8
{
	Length UMETA(DisplayName = "Length"),
	Email UMETA(DisplayName = "Email"),
	Phone UMETA(DisplayName = "Phone"),
	None UMETA(DisplayName = "None"),
};

UENUM(BlueprintType)
enum class ECustomTransitionType : uint8
{
	Linear UMETA(DisplayName = "Linear"),
	EaseInOut UMETA(DisplayName = "EaseInOut"),
	EaseIn UMETA(DisplayName = "EaseIn"),
	EaseOut UMETA(DisplayName = "EaseOut"),
	Curve UMETA(DisplayName = "Curve"),
};

/**
 * STRUCTS
 */

USTRUCT(BlueprintType)
struct FTransition
{
	GENERATED_BODY()

public:
	FTransition() = default;

	FTransition(
		const float InDuration,
		const float InDelay,
		const int32 InRepeatTimes,
		const bool bInYoYo,
		const ECustomTransitionType InTransitionType,
		const float InLoopDelay = 0.0f)
		: Duration(InDuration),
		  Delay(InDelay),
		  LoopDelay(InLoopDelay),
		  RepeatTimes(InRepeatTimes),
		  bYoYo(bInYoYo),
		  TransitionType(InTransitionType)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float Duration = 0.25f;

	/** Delay before the transition begins for the first time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float Delay = 0.0f;

	/** Delay after a completed cycle before the next repeated cycle begins. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float LoopDelay = 0.0f;

	/** -1 plays once, 0 repeats indefinitely, and positive values are the number of additional cycles. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition", meta=(ClampMin="-1", UIMin="-1"))
	int32 RepeatTimes = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	bool bYoYo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	ECustomTransitionType TransitionType = ECustomTransitionType::Linear;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Transition",
		meta = (EditCondition = "TransitionType == ECustomTransitionType::Curve", EditConditionHides))
	TObjectPtr<UCurveFloat> Curve = nullptr;
};

USTRUCT(BlueprintType)
struct FWordListStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Words")
	FString Words;
};

/**
 * EMPTY CLASS
 */

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class TOGETHER_API USharedTypes : public UObject
{
	GENERATED_BODY()
};
