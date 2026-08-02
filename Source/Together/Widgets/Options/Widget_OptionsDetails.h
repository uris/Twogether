// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Image/ImageBuilder.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widget_OptionsDetails.generated.h"

class UScrollBox;
class UVerticalBox;
class USpacer;
class UScaleBox;
class USizeBox;
class UOptionsListItemDataObject_Base;
class URichTextBlock;
class UCommonTextBlock;
class UCommonLazyImage;
/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TOGETHER_API UWidget_OptionsDetails : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Apperance")
	float VerticalGap = 16.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Apperance")
	float BottomPadding = 24.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Apperance")
	float MaxDesiredHeight = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Apperance")
	float MaxDesiredImageHeight = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Apperance")
	TEnumAsByte<EStretch::Type> ImageFill = EStretch::ScaleToFit;

	void UpdateDetailsView(const UOptionsListItemDataObject_Base* InListItemData,
	                       const FString& InWidgetClassName);

	void ClearDetailsView();

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UCommonTextBlock> Title;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UVerticalBox> TitleWrapper;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USizeBox> Underline;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USizeBox> ImageSizeBox;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UScrollBox> ContentScrollBox;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USizeBox> DetailsSizeBox;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UNamedSlot> WidgetSlot;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UCommonLazyImage> Image;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<UScaleBox> ImageWrapper;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USpacer> ImageSpacer;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USpacer> DescriptionSpacer;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USpacer> MessageSpacer;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<USpacer> BottomSpacer;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<URichTextBlock> Description;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<URichTextBlock> Message;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = true))
	TObjectPtr<URichTextBlock> DebugInfo;

	// helpers
	void SetTitle(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetImage(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetDescription(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetMessage(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetDebugInfo(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetUnderline(bool bShowUnderline) const;
	void SetWidget(const UOptionsListItemDataObject_Base* InListItemData = nullptr);
	void ApplyLayout();
	void ClearWidget();

	// cached loaded widget
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> LoadedOptionalWidget;
};
