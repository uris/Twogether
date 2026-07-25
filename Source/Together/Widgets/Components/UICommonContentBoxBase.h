// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/BackgroundBlur.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "SharedTypes/SharedTypes.h"
#include "UICommonContentBoxBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUICommonContentBoxTransitionEvent, bool, bIsCollapsed);

/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonContentBoxBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FUICommonContentBoxTransitionEvent OnTransitionStart;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FUICommonContentBoxTransitionEvent OnTransitionEnd;

	UPROPERTY(meta = (BindWidget))
	USizeBox* ContentSizeBox;

	UPROPERTY(meta = (BindWidget))
	USizeBox* TopBorderSize;

	UPROPERTY(meta = (BindWidget))
	USizeBox* BottomBorderSize;

	UPROPERTY(meta = (BindWidget))
	UBorder* ContentBorder;

	UPROPERTY(meta = (BindWidget))
	UBackgroundBlur* ContentBackground;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ContentScrollBox;

	UPROPERTY(meta = (BindWidget))
	UNamedSlot* ContentSlot;

	UPROPERTY(meta = (BindWidget))
	UBorder* TopBorder;

	UPROPERTY(meta = (BindWidget))
	UBorder* BottomBorder;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Size")
	float Width;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Size")
	float MinWidth = 400.0f;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Size")
	float Height;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Size")
	float MinHeight = 400.f;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	float BorderSize = 3.0f;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	FLinearColor BorderColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.3f);

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	FLinearColor BackgroundColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere,
		Category="Custom Behavior|Appearance",
		meta=(ClampMin="0.0", ClampMax="100.0", UIMin="0.0", UIMax="100.0"))
	float BackgroundBlurStrength = 50.0f;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	FMargin ContentInnerPadding = FMargin(0.f);

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	TEnumAsByte<EHorizontalAlignment> ContentHorizontalJustification = EHorizontalAlignment::HAlign_Fill;

	UPROPERTY(EditAnywhere, Category="Custom Behavior|Appearance")
	TEnumAsByte<EVerticalAlignment> ContentVerticalJustification = EVerticalAlignment::VAlign_Fill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|State")
	bool bCollapsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|State")
	bool bHidden = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|State")
	bool bScrolls = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Animation")
	FTransition Transition = FTransition();

	UFUNCTION(BlueprintCallable, Category="Custom Behavior|State")
	void SetCollapsed(bool bCollapse, bool bImmediately = false);

	UFUNCTION(BlueprintCallable, Category="Custom Behavior|State")
	void SetHidden(bool bHide);

	UFUNCTION(BlueprintCallable, Category="Custom Behavior|State")
	void SetScrolls(bool bEnableScrolling);

	UFUNCTION(BlueprintCallable, Category="Custom Behavior|State")
	void SetVerticalScrollOffset(float Offset) const;

	/** Restores the state configured on this widget before its first runtime construction. */
	UFUNCTION(BlueprintCallable, Category="Custom Behavior|State")
	void RestoreInitialSettings();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void ApplyWidgetSettings();
	void PerformCollapseTransition(bool bCollapse);
	float EvaluateTransitionAlpha(float Alpha) const;
	void SetBorderVisibility(ESlateVisibility InVisibility) const;
	void SetBorderOpacity(float Opacity) const;

	bool bCollapseTransitionActive = false;
	// Tracks the destination of the active transition so duplicate requests can be ignored
	// while opposite requests can safely reverse from the current visual state.
	bool bCollapseTransitionTarget = false;
	bool bAnimateHeightDuringTransition = true;
	float CollapseTransitionElapsed = 0.0f;
	float CollapseTransitionStartHeight = 0.0f;
	float CollapseTransitionTargetHeight = 0.0f;
	float BorderTransitionStartOpacity = 1.0f;
	float BorderTransitionTargetOpacity = 1.0f;

	bool bInitialSettingsCaptured = false;
	bool bInitialCollapsed = false;
	bool bInitialHidden = false;
	bool bInitialScrolls = false;
};