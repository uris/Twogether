// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SharedTypes/MotionValue.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "Widget_PressAKey.generated.h"

class UHorizontalBox;
class UCommonTextBlock;

/** Native behavior and opacity motion for WBP_CAW_PressAKey. */
UCLASS()
class TOGETHER_API UWidget_PressAKey : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Press A Key|Animation")
	FTransition TitleTransition = FTransition(1.0f, 0.0f, -1, false, ECustomTransitionType::EaseOut);

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Press A Key|Animation",
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float TitleStartingOpacity = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Press A Key|Animation",
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float TitleEndingOpacity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Press A Key|Animation")
	FTransition PromptTransition = FTransition(1.0f, 0.0f, 0, true, ECustomTransitionType::EaseInOut);

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Press A Key|Animation",
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float PromptStartingOpacity = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Press A Key|Animation",
		meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float PromptEndingOpacity = 1.0f;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void PushMainMenu();
	void StartPromptMotion();

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UHorizontalBox> TitleWrapper;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UCommonTextBlock> Txt_PressKey;

	UPROPERTY()
	FMotionValue TitleOpacityMotion;

	UPROPERTY()
	FMotionValue PromptOpacityMotion;

	UPROPERTY(EditDefaultsOnly, Category = "Press A Key|Audio", meta = (Categories = "UI.SoundFX.Button"))
	FGameplayTag ContinueSound = FGameplayTag::RequestGameplayTag(TEXT("UI.SoundFX.Button.Select"));

	bool bPushPending = false;
};
