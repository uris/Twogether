// Uris - All Rights Reserved


#include "Widgets/Options/ListView/Widget_ListEntry_String.h"

#include "CommonInputSubsystem.h"
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

	ApplyStyleUpdates();
}

void UWidget_ListEntry_String::NativeDestruct()
{
	Super::NativeDestruct();
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
	ApplyStyleUpdates();
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
	ApplyStyleUpdates();

	Super::NativeListEntryWidgetHovered(bInIsHovered);
}

void UWidget_ListEntry_String::NativeListEntryWidgetSelected(const bool bInIsSelected)
{
	bIsSelected = bInIsSelected;
	ApplyStyleUpdates();

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

void UWidget_ListEntry_String::ApplyStyleUpdates() const
{
	if ((bIsSelected || bIsHovered) && HoveredTextStyle)
	{
		if (SettingDisplayName)
		{
			SettingDisplayName->SetStyle(HoveredTextStyle);
		}
		if (SettingRotator)
		{
			SettingRotator->SetTextStyle(HoveredTextStyle);
		}
	}
	else if (DefaultTextStyle)
	{
		if (SettingDisplayName)
		{
			SettingDisplayName->SetStyle(DefaultTextStyle);
		}
		if (SettingRotator)
		{
			SettingRotator->SetTextStyle(DefaultTextStyle);
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
