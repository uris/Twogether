// Uris - All Rights Reserved


#include "ListItemDataObject_Scalar.h"

#include "Widgets/Options/OptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions UListItemDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;

	return Options;
}

FCommonNumberFormattingOptions UListItemDataObject_Scalar::Decimal(const int32 FractionalDigits)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = FractionalDigits;

	return Options;
}

float UListItemDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return FMath::GetMappedRangeValueClamped(OutputRange,
		                                         ValueRange,
		                                         StringToFloat(DataDynamicGetter->GetValueAsString()));
	}

	return 1.0f;
}

void UListItemDataObject_Scalar::SetCurrentValue(const float InValue)
{
	if (DataDynamicSetter)
	{
		const float Clamped = FMath::GetMappedRangeValueClamped(ValueRange, OutputRange, InValue);
		DataDynamicSetter->SetValueFromString(FloatToString(Clamped));
		NotifyListDataModified(this);
	}
}

float UListItemDataObject_Scalar::StringToFloat(const FString& InString)
{
	float OutValue = 0.0f;
	LexFromString(OutValue, *InString);
	return OutValue;
}

FString UListItemDataObject_Scalar::FloatToString(const float InValue)
{
	return LexToString(InValue);
}

bool UListItemDataObject_Scalar::CanResetBackToDefault() const
{
	if (HasDefaultValue() && DataDynamicGetter)
	{
		const float DefaultVal = StringToFloat(GetDefaultValueAsString());
		const float CurrentVal = GetCurrentValue();
		return !FMath::IsNearlyEqual(DefaultVal, CurrentVal, 0.01);
	}

	return false;
}

bool UListItemDataObject_Scalar::ResetToDefault()
{
	if (!CanResetBackToDefault())
	{
		return false;
	}

	if (!DataDynamicSetter)
	{
		return false;
	}

	const FString DefaultVal = GetDefaultValueAsString();
	DataDynamicSetter->SetValueFromString(DefaultVal);
	NotifyListDataModified(this, EOptionsListModifiedReason::ResetToDefault);

	return true;
}
