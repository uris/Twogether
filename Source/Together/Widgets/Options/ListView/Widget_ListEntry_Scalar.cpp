// Uris - All Rights Reserved


#include "Widget_ListEntry_Scalar.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "ListEntryStyle.h"
#include "Components/VerticalBox.h"
#include "Widgets/Components/UICommonTextBase.h"
#include "Widgets/Components/UISliderBase.h"
#include "Widgets/Options/DataObjects/ListItemDataObject_Scalar.h"

void UWidget_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Slider->Slider)
	{
		Slider->Slider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleOnValueChanged);
		Slider->Slider->OnMouseCaptureBegin.
		        AddUniqueDynamic(this, &UWidget_ListEntry_Scalar::HandleOnMouseCaptureBegin);
	}
}

void UWidget_ListEntry_Scalar::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyStyles();

	if (Slider && IsValid(ListEntryStyle))
	{
		Slider->UpdateTextStyles(ListEntryStyle->ValueTextStyle);
	}
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
		TGuardValue<bool> UpdatingGuard(bUpdatingFromDataObject, true);
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
		TGuardValue<bool> UpdatingGuard(bUpdatingFromDataObject, true);
		Slider->Slider->SetValue(CachedOwningScalarObject->GetCurrentValue());
	}
}

FReply UWidget_ListEntry_Scalar::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// handle focus on slider
	UCommonInputSubsystem* IS = GetInputSubsystem();
	if (IS && IS->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if (Slider->Slider)
		{
			return FReply::Handled().SetUserFocus(Slider->Slider->GetCachedWidget().ToSharedRef());
		}
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UWidget_ListEntry_Scalar::NativeListEntryWidgetHovered(const bool bInIsHovered)
{
	if (bIsHovered == bInIsHovered)
	{
		return;
	}
	bIsHovered = bInIsHovered;
	ApplyStyles();

	Super::NativeListEntryWidgetHovered(bInIsHovered);
}

void UWidget_ListEntry_Scalar::NativeListEntryWidgetSelected(const bool bInIsSelected)
{
	bIsSelected = bInIsSelected;
	ApplyStyles();

	Super::NativeListEntryWidgetSelected(bInIsSelected);
}

void UWidget_ListEntry_Scalar::ApplyEditabilityToControls(const bool bInIsEditable)
{

	// update the slider's enabled state
	if (Slider->Slider)
	{
		Slider->Slider->SetIsEnabled(bInIsEditable);
	}

	ApplyStyles();
}

void UWidget_ListEntry_Scalar::HandleOnValueChanged(const float InVolume) const
{
	if (!bUpdatingFromDataObject && IsValid(CachedOwningScalarObject))
	{
		CachedOwningScalarObject->SetCurrentValue(InVolume);
	}
}

void UWidget_ListEntry_Scalar::HandleOnMouseCaptureBegin()
{
	bIsSelected = true;
	bIsHovered = true;
	ApplyStyles();
	RequestOwningItemSelection(); // request row item gets selected
}

void UWidget_ListEntry_Scalar::ApplyStyles()
{
	Super::ApplyStyles();

	if (!IsValid(ListEntryStyle))
	{
		return;
	}

	const FListTextStyle EntryStyle = ListEntryStyle->ItemTextStyle;

	// if disabled, set the disabled style
	if (!bIsEditable)
	{
		if (SettingDisplayName && EntryStyle.DisabledTextStyle)
		{
			SettingDisplayName->SetStyle(EntryStyle.DisabledTextStyle);
		}
		return;
	}

	// otherwise set normal or hovered
	if ((bIsSelected || bIsHovered) && EntryStyle.HoveredTextStyle)
	{
		if (SettingDisplayName)
		{
			SettingDisplayName->SetStyle(EntryStyle.HoveredTextStyle);
		}
	}
	else if (EntryStyle.DefaultTextStyle)
	{
		if (SettingDisplayName)
		{
			SettingDisplayName->SetStyle(EntryStyle.DefaultTextStyle);
		}
	}

	if (Slider)
	{
		Slider->UpdateSliderStyle(bIsSelected, bIsHovered);
	}
}
