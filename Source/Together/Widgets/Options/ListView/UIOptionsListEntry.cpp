// Uris - All Rights Reserved


#include "Widgets/Options/ListView/UIOptionsListEntry.h"

#include "CommonTextBlock.h"
#include "Utility/Debug.h"
#include "Widgets/Components/UICommonTextBase.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"

void UUIOptionsListEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	CachedListItemObject = ListItemObject;

	SetVisibility(ESlateVisibility::Visible); // ( default is collapsed)

	OnOwningListDataObjectSet(CastChecked<UOptionsListItemDataObject_Base>(ListItemObject));
}

void UUIOptionsListEntry::NativeOnEntryReleased()
{
	OnEntrySelectionRequested.Clear();
	CachedListItemObject = nullptr;

	IUserObjectListEntry::NativeOnEntryReleased();
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
}

void UUIOptionsListEntry::OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
                                                         EOptionsListModifiedReason InReason)
{
	// override
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
