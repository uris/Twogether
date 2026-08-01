// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnalogSlider.h"
#include "CommonNumericTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "UISliderBase.generated.h"

struct FListTextStyle;
class USizeBox;
class UImage;
/**
 *
 */
UCLASS()
class TOGETHER_API UUISliderBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Track")
	float TrackHeight = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Track")
	FLinearColor TrackColor = FLinearColor({1.0f, 1.0f, 1.0f, 0.25f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Progress Bar")
	FLinearColor BarColor = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Progress Bar")
	FLinearColor BarColorHovered = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Progress Bar")
	FLinearColor BarColorSelected = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Handle")
	float HandleSize = 0.0f;

	UPROPERTY(BlueprintReadWrite,
		EditAnywhere,
		Category = "Custom Properties|Handle",
		meta=(EditCondition="HandleSize > 0"))
	FLinearColor HandleColor = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite,
		EditAnywhere,
		Category = "Custom Properties|Handle",
		meta=(EditCondition="HandleSize > 0"))
	FLinearColor HandleColorHovered = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite,
		EditAnywhere,
		Category = "Custom Properties|Handle",
		meta=(EditCondition="HandleSize > 0"))
	FLinearColor HandleColorSelected = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Text Label")
	bool bShowValue = true;

	UPROPERTY(BlueprintReadWrite,
		EditAnywhere,
		Category = "Custom Properties|Text Label",
		meta=(EditCondition="bShowValue"))
	float ValueBoxSize = 85.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> ValueDefaultTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> ValueHoveredTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> ValueSelectedTextStyle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties|Text Styles")
	TSubclassOf<UCommonTextStyle> ValueDisabledTextStyle;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UAnalogSlider> Slider;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UCommonNumericTextBlock> SliderValue;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UImage> Progress;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> SliderValueSizeBox;

	UFUNCTION(BlueprintCallable)
	void UpdateSliderStyle(bool bIsSelected, bool bIsHovered) const;

	UFUNCTION(BlueprintCallable)
	void UpdateTextStyles(const FListTextStyle& InTextStyles);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ApplyProgress() const;

	UFUNCTION()
	void HandleSliderValueChanged(float Value) const;

private:
	float CurrentSliderWidth = INDEX_NONE;
};
