// Uris - All Rights Reserved


#include "Widget_ListEntry_Scalar.h"

#include "Widgets/Components/UISliderBase.h"
#include "Widgets/Options/DataObjects/ListItemDataObject_Scalar.h"

void UWidget_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Slider->Slider)
	{
		Slider->Slider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleOnValueChanged);
	}
}

void UWidget_ListEntry_Scalar::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWidget_ListEntry_Scalar::OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	UListItemDataObject_Scalar* ScalarWidget = Cast<UListItemDataObject_Scalar>(InOwningListDataObject);
	if (IsValid(ScalarWidget))
	{
		CachedOwningScalarObject = ScalarWidget;
	}
	if (Slider->SliderValue)
	{
		// update numeric value
		Slider->SliderValue->SetNumericType(CachedOwningScalarObject->GetValueType());
		Slider->SliderValue->FormattingSpecification = CachedOwningScalarObject->GetFormatting();
		Slider->SliderValue->SetCurrentValue(CachedOwningScalarObject->GetCurrentValue());
	}
	if (Slider->Slider)
	{
		// update analog slider
		Slider->Slider->SetMinValue(CachedOwningScalarObject->GetValueRange().GetLowerBoundValue());
		Slider->Slider->SetMaxValue(CachedOwningScalarObject->GetValueRange().GetUpperBoundValue());
		Slider->Slider->SetStepSize(CachedOwningScalarObject->GetSliderStepSize());
		Slider->Slider->SetValue(CachedOwningScalarObject->GetCurrentValue());
	}
}

void UWidget_ListEntry_Scalar::OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
                                                              EOptionsListModifiedReason InReason)
{
	Super::OnOwningListDataObjectModified(InOwningListDataObject, InReason);

	if (!CachedOwningScalarObject)
	{
		return;
	}

	if (Slider->SliderValue)
	{
		Slider->SliderValue->SetCurrentValue(CachedOwningScalarObject->GetCurrentValue());
	}
	if (Slider->Slider)
	{
		Slider->Slider->SetValue(CachedOwningScalarObject->GetCurrentValue());
	}
}

FReply UWidget_ListEntry_Scalar::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UWidget_ListEntry_Scalar::NativeListEntryWidgetHovered(bool bInIsHovered)
{
	Super::NativeListEntryWidgetHovered(bInIsHovered);
}

void UWidget_ListEntry_Scalar::NativeListEntryWidgetSelected(const bool bInIsSelected)
{
	Super::NativeListEntryWidgetSelected(bInIsSelected);
}

void UWidget_ListEntry_Scalar::HandleOnValueChanged(const float InVolume) const
{
	if (IsValid(CachedOwningScalarObject))
	{
		CachedOwningScalarObject->SetCurrentValue(InVolume);
	}
}
