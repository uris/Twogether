// Uris - All Rights Reserved


#include "Widgets/Options/ListView/Widget_ListEntry_String.h"

#include "CommonInputSubsystem.h"
#include "ListEntryStyle.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBox.h"
#include "Utility/Debug.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "Widgets/Components/UICommonRotator.h"
#include "Widgets/Options/DataObjects/ListItemDataObject_String.h"

#include "Widgets/Options/DataObjects/ListItemDataObject_String.h"

void UWidget_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (CycleLeft)
	{
		CycleLeft->OnClicked().AddUObject(this, &ThisClass::HandleCycleLeft);
	}

	if (CycleRight)
	{
		CycleRight->OnClicked().AddUObject(this, &ThisClass::HandleCycleRight);
	}

	if (SettingRotator)
	{
		SettingRotator->OnClicked().AddUObject(this, &ThisClass::HandleRotatorClicked);
		SettingRotator->OnRotatedEvent.AddUObject(this, &ThisClass::HandleRotatedEvent);
	}

	ApplyStyles();
}

void UWidget_ListEntry_String::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyStyles();
}

void UWidget_ListEntry_String::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyButtonStyles();
}

void UWidget_ListEntry_String::NativeDestruct()
{
	Super::NativeDestruct();
}

void UWidget_ListEntry_String::NativeOnStateChange(const EStateChangeType StateChangeType, const bool bStateValue)
{
	Super::NativeOnStateChange(StateChangeType, bStateValue);

	UE_LOG(LogTemp, Warning, TEXT("State Change"))
	ApplyButtonStyles();
}

void UWidget_ListEntry_String::OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningListDataObject = CastChecked<UListItemDataObject_String>(InOwningListDataObject);
	const TArray<FText> AvailableOptions = CachedOwningListDataObject->GetAvailableDisplayOptionsArray();

	// set labels for rotator
	SettingRotator->PopulateTextLabels(AvailableOptions);

	// display the currently selected item
	SettingRotator->SetSelectedOptionByText(CachedOwningListDataObject->GetCurrentDisplayText());

	// update styles
	ApplyStyles();
}

void UWidget_ListEntry_String::OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
                                                              const EOptionsListModifiedReason InReason)
{
	Super::OnOwningListDataObjectModified(InOwningListDataObject, InReason);

	if (CachedOwningListDataObject)
	{
		// display the currently selected item
		SettingRotator->SetSelectedOptionByText(CachedOwningListDataObject->GetCurrentDisplayText());
	}
}

void UWidget_ListEntry_String::ApplyEditabilityToControls(const bool bInIsEditable)
{
	// update the rotator-enabled state
	if (SettingRotator)
	{
		SettingRotator->SetIsEnabled(bInIsEditable);
	}
	if (CycleLeft)
	{
		CycleLeft->SetIsEnabled(bInIsEditable);
	}
	if (CycleRight)
	{
		CycleRight->SetIsEnabled(bInIsEditable);
	}

	// update styles
	ApplyStyles();
}

FReply UWidget_ListEntry_String::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	// handle focus on Rotator for gamepad support to cycle options with left/right arrows
	UCommonInputSubsystem* IS = GetInputSubsystem();
	if (IS && IS->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if (SettingRotator)
		{
			return FReply::Handled().SetUserFocus(SettingRotator->GetCachedWidget().ToSharedRef());
		}
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UWidget_ListEntry_String::NativeListEntryWidgetHovered(const bool bInIsHovered)
{
	if (bIsHovered == bInIsHovered)
	{
		return;
	}
	bIsHovered = bInIsHovered;
	ApplyStyles();

	Super::NativeListEntryWidgetHovered(bInIsHovered);
}

void UWidget_ListEntry_String::NativeListEntryWidgetSelected(const bool bInIsSelected)
{
	bIsSelected = bInIsSelected;
	ApplyStyles();

	Super::NativeListEntryWidgetSelected(bInIsSelected);
}

void UWidget_ListEntry_String::HandleCycleLeft() const
{
	CycleSelection(EStringSettingDirection::Previous);
}

void UWidget_ListEntry_String::HandleCycleRight() const
{
	CycleSelection(EStringSettingDirection::Next);
}

void UWidget_ListEntry_String::HandleRotatorClicked() const
{
	CycleSelection(EStringSettingDirection::Next);
}

void UWidget_ListEntry_String::HandleRotatedEvent(int32 Value, bool bUserInitiated) const
{
	UE_LOG(LogTemp, Warning, TEXT("Rotated: %d"), bUserInitiated);

	if (!IsValid(CachedOwningListDataObject))
	{
		return;
	}

	UCommonInputSubsystem* IS = GetInputSubsystem();
	if (!IS || !bUserInitiated)
	{
		return;
	}

	// use the selected text to commit updates from user selection
	if (IS->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		const FText SelectedOption = SettingRotator->GetSelectedText();
		CachedOwningListDataObject->OnRotatorInitiatedValueChange(SelectedOption);
	}
}

void UWidget_ListEntry_String::CycleSelection(const EStringSettingDirection InDirection) const
{
	// the button click consumes a pointer event: explicitly request item selection
	// important: call this before cycling to trigger selection-dependent events before updating value changes
	RequestOwningItemSelection(); // <- calls SetSelectedItem when the request trickles to the owning screen

	// process click cycle action
	if (CachedOwningListDataObject)
	{
		const FText NewVal = CachedOwningListDataObject->CycleCurrentSetting(InDirection);
	}
}

void UWidget_ListEntry_String::ApplyStyles()
{
	Super::ApplyStyles();

	if (!IsValid(ListEntryStyle))
	{
		return;
	}

	const FListTextStyle ItemEntryStyle = ListEntryStyle->ItemTextStyle;
	const FListTextStyle ValueEntryStyle = ListEntryStyle->ValueTextStyle;

	if (!bIsEditable)
	{
		if (SettingDisplayName && ItemEntryStyle.DisabledTextStyle && ItemEntryStyle.HoveredTextStyle)
		{
			SettingDisplayName->SetStyle(
				bIsHovered ? ItemEntryStyle.HoveredTextStyle : ItemEntryStyle.DisabledTextStyle);
		}
		return;
	}

	if ((bIsSelected || bIsHovered))
	{
		if (SettingDisplayName && ItemEntryStyle.HoveredTextStyle)
		{
			SettingDisplayName->SetStyle(ItemEntryStyle.HoveredTextStyle);
		}
		if (SettingRotator && ValueEntryStyle.HoveredTextStyle)
		{
			SettingRotator->SetTextStyle(ValueEntryStyle.HoveredTextStyle);
		}
	}
	else
	{
		if (SettingDisplayName && ItemEntryStyle.DefaultTextStyle)
		{
			SettingDisplayName->SetStyle(ItemEntryStyle.DefaultTextStyle);
		}
		if (SettingRotator && ValueEntryStyle.DefaultTextStyle)
		{
			SettingRotator->SetTextStyle(ValueEntryStyle.DefaultTextStyle);
		}
	}
}

void UWidget_ListEntry_String::ApplyButtonStyles() const
{
	const bool bShowButtons = bIsSelected || bIsHovered;
	if (bShowButtons)
	{
		if (CycleRight)
		{
			CycleRight->SetRenderOpacity(bIsEditable ? ButtonOpacityOn : ButtonOpacityOff);
		}
		if (CycleLeft)
		{
			CycleLeft->SetRenderOpacity(bIsEditable ? ButtonOpacityOn : ButtonOpacityOff);
		}
	}
	else
	{
		if (CycleRight)
		{
			CycleRight->SetRenderOpacity(ButtonOpacityOff);
		}
		if (CycleLeft)
		{
			CycleLeft->SetRenderOpacity(ButtonOpacityOff);
		}
	}
}

bool UWidget_ListEntry_String::IsListDataObjectOwner(UUIOptionsListEntry* InObject) const
{
	if (const UWidget_ListEntry_String* Entry = Cast<UWidget_ListEntry_String>(InObject))
	{
		const FString EntryDataId = Entry->GetOwningListDataObject()->GetDataId().ToString();
		const FString ThisDataId = GetOwningListDataObject()->GetDataId().ToString();
		return EntryDataId.Equals(ThisDataId);
	}
	return false;
}
