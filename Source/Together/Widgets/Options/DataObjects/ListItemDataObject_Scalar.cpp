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
	const float FallbackOutputValue = GetFallbackOutputValue();
	float OutputValue = FallbackOutputValue;
	bool bShouldRepairStoredValue = false;

	if (DataDynamicGetter)
	{
		const FString StoredValue = DataDynamicGetter->GetValueAsString();
		if (!TryStringToFloat(StoredValue, OutputValue) || !FMath::IsFinite(OutputValue))
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Scalar setting '%s' contains invalid value '%s'; using '%s' instead."),
				*GetDataId().ToString(),
				*StoredValue,
				*FloatToString(FallbackOutputValue));
			OutputValue = FallbackOutputValue;
			bShouldRepairStoredValue = true;
		}
	}

	const float ClampedOutputValue = FMath::Clamp(
		OutputValue,
		OutputRange.GetLowerBoundValue(),
		OutputRange.GetUpperBoundValue());
	bShouldRepairStoredValue |= !FMath::IsNearlyEqual(OutputValue, ClampedOutputValue);

	if (bShouldRepairStoredValue && DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(FloatToString(ClampedOutputValue));
	}

	return FMath::GetMappedRangeValueClamped(
		OutputRange,
		ValueRange,
		ClampedOutputValue);
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

bool UListItemDataObject_Scalar::TryStringToFloat(const FString& InString, float& OutValue)
{
	return LexTryParseString(OutValue, *InString);
}

FString UListItemDataObject_Scalar::FloatToString(const float InValue)
{
	return LexToString(InValue);
}

bool UListItemDataObject_Scalar::CanResetBackToDefault() const
{
	if (HasDefaultValue() && DataDynamicGetter)
	{
		float DefaultOutputValue = 0.0f;
		if (!TryStringToFloat(GetDefaultValueAsString(), DefaultOutputValue) ||
			!FMath::IsFinite(DefaultOutputValue))
		{
			return false;
		}

		const float DefaultValue = FMath::GetMappedRangeValueClamped(
			OutputRange,
			ValueRange,
			DefaultOutputValue);
		return !FMath::IsNearlyEqual(DefaultValue, GetCurrentValue(), KINDA_SMALL_NUMBER);
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

	float DefaultOutputValue = 0.0f;
	if (!TryStringToFloat(GetDefaultValueAsString(), DefaultOutputValue) ||
		!FMath::IsFinite(DefaultOutputValue))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Unable to reset scalar setting '%s': default value '%s' is invalid."),
			*GetDataId().ToString(),
			*GetDefaultValueAsString());
		return false;
	}

	DefaultOutputValue = FMath::Clamp(
		DefaultOutputValue,
		OutputRange.GetLowerBoundValue(),
		OutputRange.GetUpperBoundValue());
	DataDynamicSetter->SetValueFromString(FloatToString(DefaultOutputValue));
	NotifyListDataModified(this, EOptionsListModifiedReason::ResetToDefault);

	return true;
}

void UListItemDataObject_Scalar::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	// Resolve and repair malformed or out-of-range saved data before the row is displayed.
	GetCurrentValue();
}

float UListItemDataObject_Scalar::GetFallbackOutputValue() const
{
	float DefaultOutputValue = OutputRange.GetLowerBoundValue();
	if (HasDefaultValue())
	{
		float ParsedDefaultValue = 0.0f;
		if (TryStringToFloat(GetDefaultValueAsString(), ParsedDefaultValue) &&
			FMath::IsFinite(ParsedDefaultValue))
		{
			DefaultOutputValue = ParsedDefaultValue;
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Scalar setting '%s' has invalid default value '%s'; using the output minimum."),
				*GetDataId().ToString(),
				*GetDefaultValueAsString());
		}
	}

	return FMath::Clamp(
		DefaultOutputValue,
		OutputRange.GetLowerBoundValue(),
		OutputRange.GetUpperBoundValue());
}
