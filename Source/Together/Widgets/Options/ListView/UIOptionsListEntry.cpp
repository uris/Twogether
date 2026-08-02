// Uris - All Rights Reserved


#include "Widgets/Options/ListView/UIOptionsListEntry.h"

#include "CommonInputSubsystem.h"
#include "CommonTextBlock.h"
#include "ListEntryStyle.h"
#include "Together.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Utility/Debug.h"
#include "Widgets/Components/UICommonTextBase.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"
#include "Widgets/Options/DataObjects/UOptionsListItemCollection_Base.h"

void UUIOptionsListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	bIsHovered = false;
	bIsSelected = false;

	CachedListItemObject = ListItemObject;

	// test for collection class type to set its visibility to not selectable / interactable
	const bool IsCollectionType = IsValid(Cast<UUOptionsListItemCollection_Base>(ListItemObject));
	SetVisibility(IsCollectionType ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Visible);

	// trigger call to method to handle tasks after a data object is known to exist
	OnOwningListDataObjectSet(CastChecked<UOptionsListItemDataObject_Base>(ListItemObject));
}

void UUIOptionsListEntry::NativeOnEntryReleased()
{

	if (UOptionsListItemDataObject_Base* PreviousItem =
		Cast<UOptionsListItemDataObject_Base>(CachedListItemObject))
	{
		PreviousItem->OnListDataModified.RemoveAll(this);
		PreviousItem->OnEditabilityChanged.RemoveAll(this);
	}

	OnEntrySelectionRequested.Clear();
	CachedListItemObject = nullptr;

	NativeListEntryWidgetHovered(false);
	NativeListEntryWidgetSelected(false);

	IUserObjectListEntry::NativeOnEntryReleased();
}

FReply UUIOptionsListEntry::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UUIOptionsListEntry::OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject)
{
	if (SettingDisplayName)
	{
		SettingDisplayName->SetText(InOwningListDataObject->GetDisplayName());
	}

	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModified.AddUObject(this, &ThisClass::OnOwningListDataObjectModified);
	}

	if (!InOwningListDataObject->OnEditabilityChanged.IsBoundToObject(this))
	{
		InOwningListDataObject->OnEditabilityChanged.AddUObject(
			this,
			&ThisClass::HandleEditabilityChanged);
	}

	if (InOwningListDataObject)
	{
		SetIndent(InOwningListDataObject->GetbIsChildEntry());
		if (InOwningListDataObject->GetbIsLastEntry())
		{
			SetBorderVisibility(false, true);
		}
	}

	// call handle editable to immediately update the visual state of the entry
	HandleEditabilityChanged(InOwningListDataObject->IsEditable());
}

void UUIOptionsListEntry::OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
                                                         EOptionsListModifiedReason InReason)
{
	// override
}

void UUIOptionsListEntry::HandleEditabilityChanged(const bool bInIsEditable)
{
	if (bIsEditable == bInIsEditable)
	{
		return;
	}

	// set state
	bIsEditable = bInIsEditable;

	// broadcast to children overriding
	ApplyEditabilityToControls(bIsEditable);
}

void UUIOptionsListEntry::ApplyEditabilityToControls(bool bInIsEditable)
{
	// implemented by children
}

void UUIOptionsListEntry::NativeOnItemSelectionChanged(const bool bInIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bInIsSelected);
	NativeListEntryWidgetSelected(bInIsSelected);
}

void UUIOptionsListEntry::RequestOwningItemSelection() const
{
	if (CachedListItemObject)
	{
		OnEntrySelectionRequested.Broadcast(CachedListItemObject);
	}
}


void UUIOptionsListEntry::NativeOnMouseEnter(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	NativeListEntryWidgetHovered(true);
}

void UUIOptionsListEntry::NativeOnMouseLeave(
	const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	NativeListEntryWidgetHovered(false);
}

void UUIOptionsListEntry::NativeListEntryWidgetHovered(const bool bInIsHovered)
{
	BP_NativeOnHovered(bInIsHovered);
}

void UUIOptionsListEntry::NativeListEntryWidgetSelected(bool bInIsSelected)
{
	BP_NativeOnSelected(bInIsSelected);
}

void UUIOptionsListEntry::ApplyStyles()
{
	// empty
}

void UUIOptionsListEntry::SetIndent(const bool bHasParent) const
{
	if (EntryWrapper && ListEntryStyle)
	{
		if (UOverlaySlot* WrapperSlot = Cast<UOverlaySlot>(EntryWrapper->Slot))
		{
			WrapperSlot->SetPadding(FMargin(
				bHasParent ? ListEntryStyle->IndentPadding : ListEntryStyle->DefaultPadding,
				0.0f,
				ListEntryStyle->DefaultPadding,
				0.0f));
		}
	}
}

void UUIOptionsListEntry::SetBorderVisibility(const bool bBottomVisible, const bool bTopVisible) const
{
	if (EntryBorderBottom)
	{
		EntryBorderBottom->SetVisibility(
			bBottomVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
	if (EntryBorderTop)
	{
		EntryBorderTop->SetVisibility(
			bTopVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}
