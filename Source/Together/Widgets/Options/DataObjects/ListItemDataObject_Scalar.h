// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonNumericTextBlock.h"
#include "ListItemDataObject_Value.h"
#include "ListItemDataObject_Scalar.generated.h"

enum class ECommonNumericType : uint8;
/**
 *
 */
UCLASS()
class TOGETHER_API UListItemDataObject_Scalar : public UListItemDataObject_Value
{
	GENERATED_BODY()

public:
	LIST_DATA_ACCESSOR(TRange<float>, ValueRange)
	LIST_DATA_ACCESSOR(ECommonNumericType, ValueType)
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, Formatting)
	LIST_DATA_ACCESSOR(TRange<float>, OutputRange)
	LIST_DATA_ACCESSOR(float, SliderStepSize)

	static FCommonNumberFormattingOptions NoDecimal();
	static FCommonNumberFormattingOptions Decimal(int32 FractionalDigits);
	float GetCurrentValue() const;
	void SetCurrentValue(float InValue);

private:
	// helper - convert string to float / float to string
	static float StringToFloat(const FString& InString);
	static FString FloatToString(float InValue);

	TRange<float> ValueRange = TRange<float>(0.0f, 1.0f);
	ECommonNumericType ValueType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions Formatting;
	TRange<float> OutputRange = TRange<float>(0.0f, 1.0f);
	float SliderStepSize = 0.1f;

};
