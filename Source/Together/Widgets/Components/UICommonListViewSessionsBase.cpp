// Uris - All Rights Reserved

#include "Widgets/Components/UICommonListViewSessionsBase.h"

#include "Kismet/GameplayStatics.h"

/** LIST VIEW */

void UUICommonListViewSessionsBase::SetGameSessions(const TArray<FTogetherSessionItem>& InItems)
{
	StructItems = InItems;

	ItemObjects.Reset(InItems.Num());

	bool bIsFirstItem = false;

	for (const FTogetherSessionItem& Data : InItems)
	{
		USessionListItemObject* ItemObject =
			NewObject<USessionListItemObject>(this);

		ItemObject->Data = Data;
		ItemObjects.Add(ItemObject);
	}

	// Replace the current items and update the list.
	SetListItems(ItemObjects);

	// set first item focused
	if (GetNumItems() > 0 && bFocusFirstItemOnLoad)
	{
		SetSelectedIndex(0);
		SetFocus();
		SetScrollOffset(0.0f);
	}

	// update did get results flag
	LoadIndex++;
}

void UUICommonListViewSessionsBase::DeactivateListView()
{
	ClearListItems();
	LoadIndex = 0;
}

void UUICommonListViewSessionsBase::OnItemClickedInternal(UObject* Item)
{
	// preserve UListView's normal On Item Clicked event.
	Super::OnItemClickedInternal(Item);

	// implement session click
	const USessionListItemObject* ItemObject =
		Cast<USessionListItemObject>(Item);

	if (!IsValid(ItemObject))
	{
		return;
	}

	OnSessionListItemClicked.Broadcast(ItemObject->Data);

	// notify the renderer representing the clicked item.
	if (UUISessionListEntryWidget* Entry =
		Cast<UUISessionListEntryWidget>(GetEntryWidgetFromItem(Item)))
	{
		Entry->HandleSessionClick();
	}
}

/** LIST ENTRY WIDGET */

void UUISessionListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// set values
	const USessionListItemObject* Item =
		Cast<USessionListItemObject>(ListItemObject);

	if (!IsValid(Item))
	{
		return;
	}

	SessionName->SetText(FText::FromString(Item->Data.GameName));
	SessionId->SetText(FText::FromString(Item->Data.SessionId));
	IsFull->SetText(Item->Data.bIsFull ? FText::FromString("Not Available") : FText::FromString("Available"));
	OnListItemDataSet(Item->Data);

}

void UUISessionListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ApplySelectionStyle(EItemState::Normal);
}

void UUISessionListEntryWidget::HandleSessionClick_Implementation()
{
	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound, HoverVolume);
	}
	ApplySelectionStyle(EItemState::Selected);
}

void UUISessionListEntryWidget::NativeOnItemSelectionChanged(
	const bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (bIsSelected)
	{
		if (!bSessionSelected)
		{
			if (HoverSound && !bSessionHovered)
			{
				UGameplayStatics::PlaySound2D(this, HoverSound, HoverVolume);
			}
			ApplySelectionStyle(EItemState::Hovered);
		}
	}
	else
	{
		if (!bSessionSelected)
		{
			ApplySelectionStyle(EItemState::Normal);
		}
	}
}

void UUISessionListEntryWidget::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();

	ApplySelectionStyle(EItemState::Normal);
}

void UUISessionListEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{

	if (HoverSound)
	{
		UGameplayStatics::PlaySound2D(this, HoverSound, HoverVolume);
	}

	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (!bSessionSelected)
	{
		ApplySelectionStyle(EItemState::Hovered);
	}

	bSessionHovered = true;

}

void UUISessionListEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (!bSessionSelected)
	{
		ApplySelectionStyle(EItemState::Normal);
	}

	bSessionHovered = false;

}

void UUISessionListEntryWidget::ApplySelectionStyle(const EItemState ItemState) const
{

	TSubclassOf<UCommonTextStyle> Style;
	switch (ItemState)
	{
		case EItemState::Hovered:
			Style = HoverTextStyle;
			break;
		case EItemState::Selected:
			Style = SelectedTextStyle;
			break;
		case EItemState::Normal:
			Style = NormalTextStyle;
			break;
	}

	if (!Style)
	{
		return;
	}

	if (SessionName)
	{
		SessionName->SetStyle(Style);
	}

	if (SessionId)
	{
		SessionId->SetStyle(Style);
	}

	if (IsFull)
	{
		IsFull->SetStyle(Style);
	}
}
