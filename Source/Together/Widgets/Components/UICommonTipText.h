// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UICommonTipText.generated.h"

class UCommonTextBlock;
/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonTipText : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCommonTextBlock* TipText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UImage* TipIcon;

	UFUNCTION(BlueprintCallable, Category="Text Setter")
	void SetTipText(FText Text) const;

	UFUNCTION(BlueprintCallable, Category="Text Setter")
	FText GetTipText() const;

	// overloads
	void SetTipText(const FString& Text) const;
	void SetTipText(FName Text) const;

protected:
	// override construct runtime
	virtual void NativeConstruct() override;

private:
	void SetTipTextVisibility() const;

};
