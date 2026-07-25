// Uris - All Rights Reserved


#include "UICommonTipText.h"

#include "CommonTextBlock.h"

void UUICommonTipText::SetTipText(const FString& Text) const
{
	if (TipText)
	{
		TipText->SetText(FText::FromString(Text));
		SetTipTextVisibility();
	}
}

void UUICommonTipText::SetTipText(const FName Text) const
{
	if (TipText)
	{
		TipText->SetText(FText::FromString(Text.ToString()));
		SetTipTextVisibility();
	}
}

void UUICommonTipText::NativeConstruct()
{
	Super::NativeConstruct();
	SetTipText(FText::GetEmpty());
}

void UUICommonTipText::SetTipText(const FText Text) const
{
	if (TipText)
	{
		TipText->SetText(Text);
		SetTipTextVisibility();
	}
}

void UUICommonTipText::SetTipTextVisibility() const
{
	if (TipText)
	{
		const ESlateVisibility TextVisibility = TipText->GetText().IsEmpty()
			                                        ? ESlateVisibility::Collapsed
			                                        : ESlateVisibility::Visible;
		TipText->SetVisibility(TextVisibility);
		if (TipIcon)
		{
			TipIcon->SetVisibility(TextVisibility);
		}
	}
}

FText UUICommonTipText::GetTipText() const
{
	if (TipText)
	{
		return TipText->GetText();
	}

	return FText::GetEmpty();
}
