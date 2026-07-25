// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "SharedTypes/MotionValue.h"
#include "UICommonTextBase.h"
#include "UICommonButtonBase.generated.h"

class UVerticalBox;
class UBorder;
class UImage;
class USizeBox;
class USpacer;

UENUM(BlueprintType)
enum class EAsyncButtonState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Working UMETA(DisplayName = "Working"),
};

UENUM(BlueprintType)
enum class EUIButtonStyle : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Underlined UMETA(DisplayName = "Underlined"),
	Icon UMETA(DisplayName = "Icon"),
};

UENUM(BlueprintType)
enum class EUIButtonSize : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Small UMETA(DisplayName = "Small"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonClick, const FString&, BtnName, EAsyncButtonState, BtnState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnButtonHover,
                                               const FString&,
                                               BtnName,
                                               FString,
                                               BtnDescription,
                                               bool,
                                               bIsHovered);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUIButtonTextTransitionEvent, bool, bIsWorking);

/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Style")
	EUIButtonStyle ButtonStyle = EUIButtonStyle::Underlined;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Style",
		meta = (EditCondition = "ButtonStyle == EUIButtonStyle::Icon"))
	bool bIsIconOnly = false;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Icon",
		meta=(DisplayThumbnail="true"))
	UTexture2D* IconAsset;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Icon")
	float IconSize;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Icon")
	float IconGap;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Icon")
	FLinearColor IconTintOff = FLinearColor({1.0f, 1.0f, 1.0f, 0.7f});

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Icon")
	FLinearColor IconTintOn = FLinearColor({1.0f, 1.0f, 1.0f, 1.0f});

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Underline")
	float UnderlineSize = 1.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Underline")
	float UnderlineGap = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Underline")
	FLinearColor UnderlineTintOff = FLinearColor({1.0f, 1.0f, 1.0f, 0.1f});

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Underline")
	FLinearColor UnderlineTintOn = FLinearColor({1.0f, 1.0f, 1.0f, 0.1f});

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase"))
	USoundBase* ClickSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float ClickVolume = 1.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="UI Button|Interaction",
		meta=(ClampMin="0.0", UIMin="0.0", Units="s"))
	float ClickDelay = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase"))
	USoundBase* HoverSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float HoverVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Button|Background")
	FLinearColor ButtonBackgroundOff = FLinearColor({1.0f, 1.0f, 1.0f, 0.0f});

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Button|Background")
	FLinearColor ButtonBackgroundOn = FLinearColor({1.0f, 1.0f, 1.0f, 0.0f});

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Button|Background")
	bool bTransitionBackground = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Button|Background")
	FTransition BackgroundTransition = FTransition(0.25f, 0.0f, 0, false, ECustomTransitionType::EaseIn);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Button")
	FString ButtonName = FString("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	bool bProgressButton = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button")
	EUIButtonSize ButtonSize = EUIButtonSize::Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI Button|Styles")
	TSubclassOf<UCommonButtonStyle> NormalButtonStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="UI Button|Styles")
	TSubclassOf<UCommonButtonStyle> SmallButtonStyle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|Text")
	bool bUseUpperCase = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|Text")
	FText ButtonLabel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|Text")
	FText ButtonLabelWorking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|Text")
	FText ButtonDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|Text")
	FText ButtonDescriptionWorking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI Button|State")
	EAsyncButtonState ButtonState = EAsyncButtonState::Idle;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Animation",
		meta=(EditCondition="bProgressButton", EditConditionHides))
	FTransition WorkingTextTransition = FTransition(1.0f, 0.0f, 0, true, ECustomTransitionType::Linear);

	UFUNCTION(BlueprintCallable, Category="UI Button")
	UUICommonButtonBase* SetAutoActivated(bool bDidAutoActivated);

	UFUNCTION(BlueprintCallable, Category="UI Button")
	void SetButtonText(FText InLabel) const;

	UFUNCTION(BlueprintCallable, Category="UI Button")
	void SetDescriptionText(const FText& InDescription);

	UFUNCTION(BlueprintCallable, Category="UI Button")
	void SetState();
	void SetHoverState() const;

	/** Reapplies the visual and CommonUI styles after configuration changes at runtime. */
	UFUNCTION(BlueprintCallable, Category="UI Button")
	void ApplyButtonConfiguration();

	UFUNCTION(BlueprintImplementableEvent, Category="UI Button")
	void OnChangeState(EAsyncButtonState ChangedButtonState);

	UFUNCTION(BlueprintCallable, Category="UI Button")
	void SetWorking();

	UFUNCTION(BlueprintCallable, Category="UI Button")
	void SetStatic();

	UPROPERTY(BlueprintAssignable)
	FOnButtonClick OnButtonClick;

	UPROPERTY(BlueprintAssignable)
	FOnButtonHover OnButtonHover;

	UPROPERTY(BlueprintAssignable, Category="UI Button|Animation")
	FUIButtonTextTransitionEvent OnTextTransitionStart;

	UPROPERTY(BlueprintAssignable, Category="UI Button|Animation")
	FUIButtonTextTransitionEvent OnTextTransitionEnd;

protected:
	// Updated in the editor
	virtual void NativePreConstruct() override;

	// triggered when the button built
	virtual void NativeConstruct() override;

	// on destruct
	virtual void NativeDestruct() override;

	virtual void NativeOnCurrentTextStyleChanged() override;

	// override when the button is clicked
	virtual void NativeOnClicked() override;

	// override when the button is hovered
	virtual void NativeOnHovered() override;

	// override when the button is un hovered
	virtual void NativeOnUnhovered() override;

	// CommonUI tab cycling changes selection without producing pointer hover events.
	virtual void NativeOnSelected(bool bBroadcast) override;
	virtual void NativeOnDeselected(bool bBroadcast) override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// execute click
	void ExecuteDelayedClick();

	// handler for delayed click execution
	FTimerHandle ClickDelayTimerHandle;

	// store state of click delay
	bool bClickPending = false;

	// track hover state
	bool bIsHovered = false;

	// Combined pointer-hover and CommonUI selection state used by custom interaction visuals.
	bool bInteractionHighlighted = false;

	// ignore selection click sound when auto-activating
	bool bIgnoreClickSoundFX = false;

	// helper
	void ClearDelayTimer();

	void StartWorkingTextTransition();
	void StopWorkingTextTransition();
	void RefreshInteractionState(bool bPlayFeedback);
	void SetBackgroundTransitionHovered(bool bHovered);
	void ApplyBackgroundTransition(float Progress) const;
	static void SetSizeBox(USizeBox* SizeBox, const float Height = 0, const float Width = 0);
	void SetupIcon() const;
	void SetupUnderline() const;
	void SetupLabel() const;

	bool bTextTransitionStarted = false;

	UPROPERTY()
	FMotionValue TextOpacityMotion;

	UPROPERTY()
	FMotionValue BackgroundMotion;

	bool bBackgroundTransitioningIn = false;

	// bind widget elements

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<UBorder> ButtonBackground;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<UUICommonTextBase> ButtonTextBox;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<UVerticalBox> ButtonTextWrapper;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<USizeBox> IconSizeBox;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<USpacer> IconGapSpacer;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<UImage> Icon;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<UBorder> UnderlineBorder;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<USizeBox> UnderlineSizeBox;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<USpacer> UnderlineGapSpacer;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidgetOptional, AllowPrivateAccess="true"))
	TObjectPtr<USpacer> TextPaddingSpacer;
};
