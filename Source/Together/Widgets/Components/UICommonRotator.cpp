// Uris - All Rights Reserved


#include "Widgets/Components/UICommonRotator.h"

#include "CommonTextBlock.h"

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

void UUICommonRotator::SetTextStyle(const TSubclassOf<UCommonTextStyle> InTextStyle)
{
	MyText->SetStyle(InTextStyle);
}

int32 UUICommonRotator::GetIndexByTextValue(const FText& InText) const
{
	return TextLabels.IndexOfByPredicate(
		[&InText](const FText& Label)
		{
			return Label.EqualTo(InText);
		});
}
