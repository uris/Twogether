// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_OptionsDetails.generated.h"

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
	void UpdateDetailsView(const UOptionsListItemDataObject_Base* InListItemData,
	                       const FString& InWidgetClassName) const;

	void ClearDetailsView() const;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> Underline;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonLazyImage> Image;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> DebugInfo;

	// helpers
	void SetTitle(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetImage(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetDescription(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetMessage(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetDebugInfo(const UOptionsListItemDataObject_Base* InListItemData = nullptr) const;
	void SetUnderline(bool bShowUnderline) const;

};
