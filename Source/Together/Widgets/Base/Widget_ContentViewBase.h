// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "Widgets/Components/UICommonContentBoxBase.h"
#include "Widget_ContentViewBase.generated.h"

class UUICommonContentBoxBase;
class UVerticalBox;
/**
 *
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class TOGETHER_API UWidget_ContentViewBase : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	// Bind content box widget element
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UUICommonContentBoxBase> ContentView;

	// Bind content header widget element
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> ContentViewHeader;

	// Bind content footer widget element
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> ContentViewFooter;

	// Opt in to fade header content with collapse/expand of content box
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		BlueprintSetter=SetHeaderShouldFade,
		Category="Content View Behavior|Transitions")
	bool bHeaderShouldFade = true;

	// Opt in to fade footer content with collapse/expand of content box
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		BlueprintSetter=SetFooterShouldFade,
		Category="Content View Behavior|Transitions")
	bool bFooterShouldFade = true;

	// Wait for content box to collapse transition before triggering widget deactivation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Content View Behavior|Transitions")
	bool bViewCollapsesBeforePop = true;

	// Setter for header fade flag
	UFUNCTION(BlueprintSetter, Category="Content View Behavior|Transitions")
	void SetHeaderShouldFade(bool bShouldFade);

	// Setter for footer fade flag
	UFUNCTION(BlueprintSetter, Category="Content View Behavior|Transitions")
	void SetFooterShouldFade(bool bShouldFade);

	// Setter for footer fade flag
	UFUNCTION(BlueprintGetter, Category="Content Vew|Getter")
	float GetViewHeight() const;

protected:
	// set up begin of exit transition
	virtual bool BeginExitTransition(EWidgetExitType Type) override;

	// handle back action
	virtual bool NativeOnHandleBackAction() override;

	// override for setup of transition end delegate
	virtual void NativeConstruct() override;

	// override for releasing transition delegate
	virtual void NativeDestruct() override;

	// clear timers, setup state
	virtual void NativeOnActivated() override;

	// clear timers, setup state
	virtual void NativeOnDeactivated() override;

	/** delegates need to be a UFUNCTION to run on a BP implementation */

	// Handle content box end of collapse transition
	UFUNCTION()
	void HandleContentBoxCollapse(bool bIsCollapsed);

	// Handle content box start of collapse transition
	UFUNCTION()
	void HandleContentBoxTransitionStart(bool bIsCollapsed);

private:
	// internal fade utilities for header/footer
	void StartContentChromeFade(bool bFadeOut);
	void UpdateContentChromeFade();
	void SetContentChromeFadeAlpha(float Alpha) const;
	float EvaluateContentChromeFadeAlpha(float Alpha) const;

	// timer handles for fade out
	FTimerHandle ContentChromeFadeTimerHandle;

	// state for fade in/out of header and footer
	float ContentChromeFadeStartTime = 0.0f;
	float ContentHeaderFadeStartAlpha = 1.0f;
	float ContentFooterFadeStartAlpha = 1.0f;
	float ContentChromeFadeTargetAlpha = 1.0f;
	float ContentViewHeaderOpacity = 1.0f;
	float ContentViewFooterOpacity = 1.0f;
	bool bContentChromeOpacityInitialized = false;

	// Back-driven exits always fade both chrome regions, regardless of their normal settings.
	bool bForceContentChromeFade = false;

	// helper: clear timers
	void ClearFadeTimer();

};