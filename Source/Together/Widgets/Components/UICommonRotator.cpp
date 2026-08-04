// Uris - All Rights Reserved


#include "Widgets/Components/UICommonRotator.h"

#include "CommonTextBlock.h"
#include "Widgets/Options/ListView/ListEntryStyle.h"
#include "Widgets/Options/ListView/UIOptionsListEntry.h"

bool UUICommonRotator::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	// UCommonRotator binds this delegate to its own non-virtual handler. Rebind it so
	// navigation requests are delegated to the owning widget without first changing
	// the rotator's internal selection.
	OnNavigation.BindUObject(this, &ThisClass::HandleDirectionalNavigation);
	return true;
}

TSharedPtr<SWidget> UUICommonRotator::HandleDirectionalNavigation(const EUINavigation InNavigation) const
{
	if (!IsInteractionEnabled())
	{
		return nullptr;
	}

	if (InNavigation == EUINavigation::Left)
	{
		OnDirectionalRotateEvent.Broadcast(ERotatorDirection::Left);
	}
	else if (InNavigation == EUINavigation::Right)
	{
		OnDirectionalRotateEvent.Broadcast(ERotatorDirection::Right);
	}

	return nullptr;
}

void UUICommonRotator::SetSelectedOptionByText(const FText& InTextOption)
{
	if (const int32 Index = GetIndexByTextValue(InTextOption); Index != INDEX_NONE)
	{
		SetSelectedItem(Index);
	}
	else
	{
		MyText->SetText(InTextOption);
	}
}

int32 UUICommonRotator::GetIndexByTextValue(const FText& InText) const
{
	return TextLabels.IndexOfByPredicate(
		[&InText](const FText& Label)
		{
			return Label.EqualTo(InText);
		});
}

void UUICommonRotator::UpdateTextStyles(const FListTextStyle& InTextStyles)
{
	if (InTextStyles.DefaultTextStyle)
	{
		DefaultTextStyle = InTextStyles.DefaultTextStyle;
	}
	if (InTextStyles.HoveredTextStyle)
	{
		HoveredTextStyle = InTextStyles.HoveredTextStyle;
		SelectedTextStyle = InTextStyles.HoveredTextStyle;
	}
	if (InTextStyles.DisabledTextStyle)
	{
		DisabledTextStyle = InTextStyles.DisabledTextStyle;
	}
}

void UUICommonRotator::SetTextStyle(const FListEntryState EntryState) const
{
	if (!EntryState.bEditable)
	{
		MyText->SetStyle(DisabledTextStyle);
	}
	else if (EntryState.IsActive())
	{
		MyText->SetStyle(HoveredTextStyle);
	}
	else
	{
		MyText->SetStyle(DefaultTextStyle);
	}
}
