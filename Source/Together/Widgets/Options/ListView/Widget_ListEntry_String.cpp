// Uris - All Rights Reserved


#include "Widgets/Options/ListView/Widget_ListEntry_String.h"

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
	}

	ApplyStyleUpdates();
}

void UWidget_ListEntry_String::NativeDestruct()
{
	Super::NativeDestruct();
	if (CycleLeft)
	{
		CycleLeft->OnClicked().RemoveAll(this);
	}
	if (CycleRight)
	{
		CycleRight->OnClicked().RemoveAll(this);
	}
	if (SettingRotator)
	{
		SettingRotator->OnClicked().RemoveAll(this);
	}
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
	const bool bWasSelected = bIsSelected;
	bIsSelected = bInIsSelected;
	UE_LOG(LogTemp, Warning, TEXT("Was Selected: %d, Selected: %d"), bWasSelected, bIsSelected);

	if (bWasSelected != bIsSelected)
	{
		BP_NativeOnSelected(bWasSelected);
	}

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
		Debug::Print(FString::Printf(TEXT("EntryDataId: %s, ThisId: %s"), *EntryDataId, *ThisDataId));
		return EntryDataId.Equals(ThisDataId);
	}
	return false;
}
