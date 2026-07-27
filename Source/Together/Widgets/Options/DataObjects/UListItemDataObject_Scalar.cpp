// Uris - All Rights Reserved


#include "UListItemDataObject_Scalar.h"

#include "Widgets/Options/OptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions UUListItemDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;

	return Options;
}

FCommonNumberFormattingOptions UUListItemDataObject_Scalar::Decimal(const int32 FractionalDigits)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = FractionalDigits;

	return Options;
}

float UUListItemDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return FMath::GetMappedRangeValueClamped(OutputRange,
		                                         ValueRange,
		                                         StringToFloat(DataDynamicGetter->GetValueAsString()));
	}

	return 0.0f;
}

float UUListItemDataObject_Scalar::StringToFloat(const FString& InString)
{
	float OutValue = 0.0f;
	LexFromString(OutValue, *InString);
	return OutValue;
}
