// Uris - All Rights Reserved


#include "Widgets/Components/UICommonBoundActionButtonBase.h"

#include "TimerManager.h"
#include "UICommonButtonBase.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Kismet/GameplayStatics.h"

void UUICommonBoundActionButtonBase::NativePreConstruct()
{
	Super::NativeConstruct();

	if (Text_ActionName)
	{
		Text_ActionName->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (IsDesignTime() && ActionButton)
	{
		ActionButton->SetButtonText(FText::FromString("Back"));
	}
}

void UUICommonBoundActionButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (ActionButton)
	{
		ActionButton->OnClicked().AddUObject(this, &ThisClass::NativeOnClicked);
		ActionButton->SetDescriptionText(FText::FromString(""));
	}
}

void UUICommonBoundActionButtonBase::NativeDestruct()
{

	if (ActionButton)
	{
		ActionButton->OnClicked().RemoveAll(this);
	}

	Super::NativeDestruct();

}

void UUICommonBoundActionButtonBase::UpdateInputActionWidget()
{
	Super::UpdateInputActionWidget();

	if (IsDesignTime() && ActionButton)
	{
		ActionButton->SetButtonText(FText::FromString("Back"));
		return;
	}

	if (ActionButton && Text_ActionName)
	{
		ActionButton->SetButtonText(Text_ActionName->GetText());
	}

}

void UUICommonBoundActionButtonBase::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();

	if (SizeBox && DesiredIconSize > 0.0f)
	{
		SizeBox->SetMaxDesiredHeight(DesiredIconSize);
		SizeBox->SetMaxDesiredWidth(DesiredIconSize);
		SizeBox->SetMinDesiredHeight(DesiredIconSize);
		SizeBox->SetMinDesiredWidth(DesiredIconSize);
		SizeBox->SetWidthOverride(DesiredIconSize);
		SizeBox->SetHeightOverride(DesiredIconSize);

		if (USizeBoxSlot* ContentSlot = Cast<USizeBoxSlot>(SizeBox->GetContentSlot()))
		{
			ContentSlot->SetHorizontalAlignment(HAlign_Center);
			ContentSlot->SetVerticalAlignment(VAlign_Center);
		}
	}

}

void UUICommonBoundActionButtonBase::NativeOnClicked()
{
	Super::NativeOnClicked();
}
