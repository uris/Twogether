// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AnalogSlider.h"
#include "CommonNumericTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "UISliderBase.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class TOGETHER_API UUISliderBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Style")
	float TrackHeight = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Style")
	float HandleSize = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Style")
	FLinearColor TrackColor = FLinearColor({1.0f, 1.0f, 1.0f, 0.25f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Style")
	FLinearColor BarColor = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite,
		EditAnywhere,
		Category = "Custom Properties|Style",
		meta=(EditCondition="HandleSize > 0"))
	FLinearColor HandleColor = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Custom Properties|Style")
	bool bShowValue = true;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UAnalogSlider> Slider;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UCommonNumericTextBlock> SliderValue;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UImage> Progress;

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
