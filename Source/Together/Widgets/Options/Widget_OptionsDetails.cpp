// Uris - All Rights Reserved


#include "Widget_OptionsDetails.h"

#include "CommonTextBlock.h"
#include "Components/RichTextBlock.h"
#include "CommonLazyImage.h"
#include "DataObjects/OptionsListItemDataObject_Base.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"
#include "Subsystems/UI/UISubsystem.h"

void UWidget_OptionsDetails::NativeConstruct()
{
	Super::NativeConstruct();
	ClearDetailsView();
}

void UWidget_OptionsDetails::UpdateDetailsView(const UOptionsListItemDataObject_Base* InListItemData,
                                               const FString& InWidgetClassName)
{
	SetTitle(InListItemData);
	SetImage(InListItemData);
	SetDescription(InListItemData);
	SetMessage(InListItemData);
	SetDebugInfo(InListItemData);
	SetWidget(InListItemData);
}


void UWidget_OptionsDetails::ClearDetailsView()
{
	SetTitle();
	SetImage();
	SetDescription();
	SetMessage();
	SetDebugInfo();
	ClearWidget();
}

void UWidget_OptionsDetails::SetTitle(const UOptionsListItemDataObject_Base* InListItemData) const
{
	if (!Title)
	{
		return;
	}
	if (InListItemData)
	{
		const FText Value = InListItemData->GetDisplayName();
		Title->SetText(Value.IsEmpty() ? FText::GetEmpty() : Value);
		SetUnderline(!Value.IsEmpty());
	}
	else
	{
		Title->SetText(FText::GetEmpty());
		SetUnderline(false);
	}
}

void UWidget_OptionsDetails::SetImage(const UOptionsListItemDataObject_Base* InListItemData) const
{
	if (!IsValid(Image))
	{
		return;
	}

	if (InListItemData)
	{
		if (const TSoftObjectPtr<UTexture2D> ImageAsset = InListItemData->GetDescriptionImage(); !ImageAsset.IsNull())
		{
			Image->SetBrushFromLazyTexture(ImageAsset);
			Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			Image->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
		Image->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UWidget_OptionsDetails::SetDescription(const UOptionsListItemDataObject_Base* InListItemData) const
{
	if (!Description)
	{
		return;
	}
	if (InListItemData)
	{
		const FText Value = InListItemData->GetDescription();
		Description->SetText(Value.IsEmpty() ? FText::GetEmpty() : Value);
		Description->SetVisibility(Value.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	else
	{
		Description->SetText(FText::GetEmpty());
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_OptionsDetails::SetMessage(const UOptionsListItemDataObject_Base* InListItemData) const
{
	if (!Message)
	{
		return;
	}
	if (InListItemData)
	{
		const FText Value = InListItemData->GetDisabledText();
		Message->SetText(Value.IsEmpty() ? FText::GetEmpty() : Value);
		Message->SetVisibility(Value.IsEmpty() ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	else
	{
		Message->SetText(FText::GetEmpty());
		Message->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_OptionsDetails::SetDebugInfo(const UOptionsListItemDataObject_Base* InListItemData) const
{
	if (!DebugInfo)
	{
		return;
	}

	if (const bool bShowDebug = !GetDefault<UTogetherSettings>()->bGlobalHideDebugMessages)
	{
		const FName ClassName = InListItemData ? InListItemData->GetClass()->GetFName() : FName();
		const FText DebugText = FText::Format(FText::FromString("<Debug>Class: {0}</>"), FText::FromName(ClassName));
		DebugInfo->SetText(InListItemData ? DebugText : FText::GetEmpty());
		DebugInfo->SetVisibility(DebugText.IsEmpty() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	else
	{
		DebugInfo->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWidget_OptionsDetails::SetUnderline(const bool bShowUnderline) const
{
	if (!Underline)
	{
		return;
	}
	Underline->SetVisibility(bShowUnderline ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UWidget_OptionsDetails::SetWidget(const UOptionsListItemDataObject_Base* InListItemData)
{
	ClearWidget();

	if (!InListItemData || InListItemData->GetDescriptionWidget().WidgetClass.IsNull() || !SizeBoxSlot)
	{
		return;
	}

	const TSoftClassPtr<UUserWidget> WidgetClass = InListItemData->GetDescriptionWidget().WidgetClass;
	UClass* LoadedClass = WidgetClass.LoadSynchronous();
	if (!LoadedClass)
	{
		return;
	}

	LoadedOptionalWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), LoadedClass);
	if (LoadedOptionalWidget)
	{
		SizeBoxSlot->AddChild(LoadedOptionalWidget);
		// SizeBoxSlot->SetMinDesiredHeight(InListItemData->GetDescriptionWidget().MinDesiredHeight);
		// SizeBoxSlot->SetMinDesiredWidth(InListItemData->GetDescriptionWidget().MinDesiredWidth);
		SizeBoxSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWidget_OptionsDetails::ClearWidget()
{
	if (SizeBoxSlot)
	{
		SizeBoxSlot->ClearChildren();
		SizeBoxSlot->SetVisibility(ESlateVisibility::Collapsed);
		// SizeBoxSlot->SetMinDesiredHeight(0.f);
		// SizeBoxSlot->SetMinDesiredWidth(0.f);
	}

	LoadedOptionalWidget = nullptr;
}
