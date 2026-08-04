// Uris - All Rights Reserved


#include "Widget_OptionsDetails.h"

#include "CommonTextBlock.h"
#include "Components/RichTextBlock.h"
#include "CommonLazyImage.h"
#include "UIFunctionLibrary.h"
#include "Components/NamedSlot.h"
#include "DataObjects/OptionsListItemDataObject_Base.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"
#include "Components/ScaleBox.h"
#include "Components/Spacer.h"
#include "Subsystems/UI/UISubsystem.h"

void UWidget_OptionsDetails::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyLayout();
}

void UWidget_OptionsDetails::NativeConstruct()
{
	Super::NativeConstruct();
	ClearDetailsView();
}

void UWidget_OptionsDetails::UpdateDetailsView(UOptionsListItemDataObject_Base* InListItemData,
                                               const FString& InWidgetClassName)
{

	if (!InListItemData || IsSameDetailsDataObject(InListItemData))
	{
		return;
	}

	CurrentDetailsDataObject = InListItemData;

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
	if (!IsValid(Image) || !ImageSpacer || !ImageWrapper)
	{
		return;
	}

	bool bHasImageAsset = false;

	if (InListItemData)
	{
		if (const TSoftObjectPtr<UTexture2D> ImageAsset = InListItemData->GetDescriptionImage(); !ImageAsset.IsNull())
		{
			Image->SetBrushFromLazyTexture(ImageAsset);
			Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			bHasImageAsset = true;
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

	if (ImageSizeBox)
	{
		ImageSizeBox->SetVisibility(bHasImageAsset ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ImageSpacer)
	{
		ImageSpacer->SetVisibility(bHasImageAsset ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ImageWrapper)
	{
		ImageWrapper->SetVisibility(bHasImageAsset ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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
		const FText Value = UUIFunctionLibrary::FormatRichText(InListItemData->GetDescription());
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
		const FString MessageText = TEXT("<Message>") + InListItemData->GetDisabledText().ToString() + TEXT("</>");
		const FText Value = UUIFunctionLibrary::FormatRichText(MessageText);
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
	const bool bHideDebug = GetDefault<UTogetherSettings>()->bGlobalHideDebugMessages;

	if (!DebugInfo || !InListItemData || bHideDebug)
	{
		DebugInfo->SetText(FText::GetEmpty());
		DebugInfo->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const FName ClassName = InListItemData
		                        ? InListItemData->GetClass()->GetFName()
		                        : FName(TEXT("Unable to get class name"));
	const FText DebugText = FText::Format(FText::FromString("<Debug>Class: {0}</>"), FText::FromName(ClassName));
	DebugInfo->SetText(DebugText);

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

	if (!InListItemData || InListItemData->GetDescriptionWidget().WidgetClass.IsNull())
	{
		ContentScrollBox->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!WidgetSlot)
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
		ContentScrollBox->SetVisibility(ESlateVisibility::Collapsed);
		WidgetSlot->AddChild(LoadedOptionalWidget);
		WidgetSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWidget_OptionsDetails::ApplyLayout()
{
	if (DetailsSizeBox)
	{
		DetailsSizeBox->SetMaxDesiredHeight(MaxDesiredHeight);
	}
	if (ImageWrapper)
	{
		ImageWrapper->SetStretch(ImageFill);
	}
	if (ImageSpacer)
	{
		ImageSpacer->SetSize({0.f, VerticalGap});
	}

	if (MessageSpacer)
	{
		MessageSpacer->SetSize({0.f, VerticalGap});
	}

	if (DescriptionSpacer)
	{
		DescriptionSpacer->SetSize({0.f, VerticalGap});
	}

	if (BottomSpacer)
	{
		BottomSpacer->SetSize({0.f, BottomPadding});
	}
}

void UWidget_OptionsDetails::ClearWidget()
{
	if (WidgetSlot)
	{
		WidgetSlot->ClearChildren();
		WidgetSlot->SetVisibility(ESlateVisibility::Collapsed);
	}

	LoadedOptionalWidget = nullptr;
}

bool UWidget_OptionsDetails::IsSameDetailsDataObject(const UOptionsListItemDataObject_Base* InDetailsObject) const
{
	if (!CurrentDetailsDataObject || !InDetailsObject)
	{
		return false;
	}
	return CurrentDetailsDataObject->GetDataId().ToString().Equals(InDetailsObject->GetDataId().ToString());
}
