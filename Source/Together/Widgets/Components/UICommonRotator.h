// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "UICommonRotator.generated.h"

struct FListEntryState;
struct FListTextStyle;
class UUICommonButtonBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonRotator : public UCommonRotator
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

	DECLARE_EVENT_OneParam(UUICommonRotator, FDirectionalRotateEvent, ERotatorDirection);

	FDirectionalRotateEvent OnDirectionalRotateEvent;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> DefaultTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> HoveredTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> SelectedTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> DisabledTextStyle;

	UFUNCTION(BlueprintCallable)
	void UpdateTextStyles(const FListTextStyle& InTextStyles);

	UFUNCTION(BlueprintCallable)
	void SetTextStyle(FListEntryState EntryState) const;

	void SetSelectedOptionByText(const FText& InTextOption);

private:
	TSharedPtr<SWidget> HandleDirectionalNavigation(EUINavigation InNavigation) const;

	int32 GetIndexByTextValue(const FText& InText) const;

};
