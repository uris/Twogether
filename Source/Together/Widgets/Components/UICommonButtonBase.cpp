// Uris - All Rights Reserved


#include "Widgets/Components/UICommonButtonBase.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Subsystems/UI/UISubsystem.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"
#include "Utility/Debug.h"

void UUICommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyButtonConfiguration();
	SetState();
}

void UUICommonButtonBase::NativeConstruct()
{
	Super::NativeConstruct();

	ClearDelayTimer();
	bClickPending = false;
	bIgnoreClickSoundFX = false;

	if (bProgressButton && ButtonState == EAsyncButtonState::Working)
	{
		StartWorkingTextTransition();
	}
}

void UUICommonButtonBase::NativeDestruct()
{
	// clear existing timers
	ClearDelayTimer();
	StopWorkingTextTransition();

	// reset sate
	bClickPending = false;
	bIgnoreClickSoundFX = false;

	// call parent
	Super::NativeDestruct();
}

void UUICommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (ButtonTextBox)
	{
		ButtonTextBox->SetStyle(GetCurrentTextStyleClass());
	}

}

void UUICommonButtonBase::NativeOnClicked()
{

	// ignore additional clicks while this click is waiting to execute.
	if (bClickPending)
	{
		return;
	}

	if (bProgressButton)
	{
		if (ButtonState == EAsyncButtonState::Working)
		{
			return;
		}
		SetWorking();
	}

	if (ClickSound)
	{
		UGameplayStatics::PlaySound2D(this, ClickSound, ClickVolume);
	}

	if (ClickDelay > 0.0f)
	{
		if (const UWorld* World = GetWorld())
		{
			bClickPending = true;
			World->GetTimerManager().SetTimer(
				ClickDelayTimerHandle,
				this,
				&ThisClass::ExecuteDelayedClick,
				ClickDelay,
				false);
			return;
		}
	}

	OnButtonClick.Broadcast(ButtonName, ButtonState);

	Super::NativeOnClicked();
}

void UUICommonButtonBase::ExecuteDelayedClick()
{
	// clear timer
	ClearDelayTimer();

	// reset state
	bClickPending = false;

	// call super click
	Super::NativeOnClicked();

	// broadcast true click
	OnButtonClick.Broadcast(ButtonName, ButtonState);
}

void UUICommonButtonBase::NativeOnHovered()
{
	bIsHovered = true;
	RefreshInteractionState(true);

	Super::NativeOnHovered();
}

void UUICommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	bIsHovered = false;
	RefreshInteractionState(false);
}

void UUICommonButtonBase::NativeOnSelected(const bool bBroadcast)
{
	Super::NativeOnSelected(bBroadcast);

	RefreshInteractionState(bBroadcast);
}

void UUICommonButtonBase::NativeOnDeselected(const bool bBroadcast)
{
	Super::NativeOnDeselected(bBroadcast);

	RefreshInteractionState(false);
}

void UUICommonButtonBase::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ButtonTextBox && TextOpacityMotion.IsPlaying())
	{
		const bool bValueChanged = TextOpacityMotion.Tick(InDeltaTime);

		if (TextOpacityMotion.DidCycleStart() && !bTextTransitionStarted)
		{
			bTextTransitionStarted = true;
			SetButtonText(ButtonLabelWorking);
			OnTextTransitionStart.Broadcast(true);
		}

		if (bValueChanged)
		{
			ButtonTextBox->SetRenderOpacity(TextOpacityMotion.GetValue());
		}

		if (TextOpacityMotion.DidComplete())
		{
			bTextTransitionStarted = false;
			OnTextTransitionEnd.Broadcast(ButtonState == EAsyncButtonState::Working);
		}
	}

	if (ButtonBackground && BackgroundMotion.IsPlaying() && BackgroundMotion.Tick(InDeltaTime))
	{
		ApplyBackgroundTransition(BackgroundMotion.GetValue());
	}
}

UUICommonButtonBase* UUICommonButtonBase::SetAutoActivated(const bool bDidAutoActivated)
{
	bIgnoreClickSoundFX = bDidAutoActivated;
	return this;
}

void UUICommonButtonBase::SetState()
{
	if (ButtonState == EAsyncButtonState::Working)
	{
		SetWorking();
	}
	else
	{
		SetStatic();
	}
}

void UUICommonButtonBase::SetHoverState() const
{
	if (ButtonBackground && !bTransitionBackground)
	{
		ButtonBackground->SetBrushColor(bInteractionHighlighted ? ButtonBackgroundOn : ButtonBackgroundOff);
	}

	if (Icon && ButtonStyle == EUIButtonStyle::Icon)
	{
		Icon->SetBrushTintColor(bInteractionHighlighted ? IconTintOn : IconTintOff);
	}

	if (UnderlineBorder && ButtonStyle == EUIButtonStyle::Underlined)
	{
		UnderlineBorder->SetBrushColor(bInteractionHighlighted ? UnderlineTintOn : UnderlineTintOff);
	}
}

void UUICommonButtonBase::SetWorking()
{
	if (!bProgressButton)
	{
		return;
	}

	ButtonState = EAsyncButtonState::Working;
	OnChangeState(ButtonState);
	StartWorkingTextTransition();
}

void UUICommonButtonBase::SetStatic()
{
	ButtonState = EAsyncButtonState::Idle;
	SetButtonText(ButtonLabel);
	OnChangeState(ButtonState);
	StopWorkingTextTransition();
}

void UUICommonButtonBase::SetButtonText(const FText InLabel) const
{
	if (!ButtonTextBox)
	{
		return;
	}
	ButtonTextBox->SetText(bUseUpperCase ? InLabel.ToUpper() : InLabel);
}

void UUICommonButtonBase::SetDescriptionText(const FText& InDescription)
{
	if (InDescription.IsEmpty())
	{
		return;
	}

	ButtonDescription = InDescription;
}

void UUICommonButtonBase::ClearDelayTimer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ClickDelayTimerHandle);
	}
}

void UUICommonButtonBase::StartWorkingTextTransition()
{
	TextOpacityMotion.Configure(1.0f, 0.0f, WorkingTextTransition);
	bTextTransitionStarted = false;

	if (ButtonTextBox)
	{
		TextOpacityMotion.Play();
		ButtonTextBox->SetRenderOpacity(TextOpacityMotion.GetValue());
	}

	if (TextOpacityMotion.IsPlaying() && FMath::Max(0.0f, WorkingTextTransition.Delay) <= KINDA_SMALL_NUMBER)
	{
		bTextTransitionStarted = true;
		SetButtonText(ButtonLabelWorking);
		OnTextTransitionStart.Broadcast(true);
	}
}

void UUICommonButtonBase::StopWorkingTextTransition()
{
	const bool bWasTransitionStarted = bTextTransitionStarted;
	TextOpacityMotion.Stop(true);
	bTextTransitionStarted = false;

	if (ButtonTextBox)
	{
		// Idle/stopped button text is always fully visible. The motion may not
		// have been configured yet during NativePreConstruct, in which case its
		// default value is zero.
		ButtonTextBox->SetRenderOpacity(1.0f);
	}

	if (bWasTransitionStarted)
	{
		OnTextTransitionEnd.Broadcast(false);
	}
}

void UUICommonButtonBase::RefreshInteractionState(const bool bPlayFeedback)
{
	const bool bShouldHighlight = bIsHovered || GetSelected();
	if (bInteractionHighlighted == bShouldHighlight)
	{
		return;
	}

	bInteractionHighlighted = bShouldHighlight;

	if (bInteractionHighlighted)
	{
		const bool bIsWorking = bProgressButton && ButtonState == EAsyncButtonState::Working;
		const FText& Description = bIsWorking ? ButtonDescriptionWorking : ButtonDescription;

		if (!bIsWorking)
		{
			if (bPlayFeedback && HoverSound && !bIgnoreClickSoundFX)
			{
				UGameplayStatics::PlaySound2D(this, HoverSound, HoverVolume);
			}
			bIgnoreClickSoundFX = false;
		}

		if (UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(this))
		{
			UISubsystem->OnButtonDescriptionUpdated.Broadcast(this, Description);
		}
		OnButtonHover.Broadcast(ButtonName, Description.ToString(), true);
	}
	else
	{
		if (UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(this))
		{
			UISubsystem->OnButtonDescriptionUpdated.Broadcast(this, FText::GetEmpty());
		}
		OnButtonHover.Broadcast(ButtonName, FString(), false);
	}

	SetHoverState();
	SetBackgroundTransitionHovered(bInteractionHighlighted);
}

void UUICommonButtonBase::SetBackgroundTransitionHovered(const bool bHovered)
{
	if (!bTransitionBackground || !ButtonBackground || bBackgroundTransitioningIn == bHovered)
	{
		return;
	}

	bBackgroundTransitioningIn = bHovered;

	if (BackgroundMotion.IsPlaying())
	{
		BackgroundMotion.Reverse(true);
	}
	else
	{
		FTransition OneShotTransition = BackgroundTransition;
		OneShotTransition.RepeatTimes = -1;
		OneShotTransition.bYoYo = false;
		BackgroundMotion.Configure(bHovered ? 0.0f : 1.0f, bHovered ? 1.0f : 0.0f, OneShotTransition);
		BackgroundMotion.Play();
	}

	ApplyBackgroundTransition(BackgroundMotion.GetValue());
}

void UUICommonButtonBase::ApplyBackgroundTransition(const float Progress) const
{
	if (!ButtonBackground)
	{
		return;
	}

	const float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
	ButtonBackground->SetRenderScale(FVector2D(ClampedProgress, 1.0f));
	ButtonBackground->SetRenderOpacity(ClampedProgress);
}

void UUICommonButtonBase::SetSizeBox(USizeBox* SizeBox, const float Height, const float Width)
{
	if (!SizeBox)
	{
		return;
	}
	if (Height > 0)
	{
		SizeBox->SetMaxDesiredHeight(Height);
		SizeBox->SetMinDesiredHeight(Height);
		SizeBox->SetHeightOverride(Height);
	}
	if (Width > 0)
	{
		SizeBox->SetMaxDesiredWidth(Width);
		SizeBox->SetWidthOverride(Width);
		SizeBox->SetMinDesiredWidth(Width);
	}
}

void UUICommonButtonBase::SetupIcon() const
{

	// set visibility
	const ESlateVisibility IconVisibility = ButtonStyle == EUIButtonStyle::Icon
		                                        ? ESlateVisibility::Visible
		                                        : ESlateVisibility::Collapsed;
	if (IconSizeBox)
	{
		IconSizeBox->SetVisibility(IconVisibility);
	}
	if (IconGapSpacer)
	{
		IconGapSpacer->SetVisibility(bIsIconOnly ? ESlateVisibility::Collapsed : IconVisibility);
	}
	if (TextPaddingSpacer)
	{
		TextPaddingSpacer->SetVisibility(bIsIconOnly ? ESlateVisibility::Collapsed : IconVisibility);
	}

	if (ButtonStyle != EUIButtonStyle::Icon)
	{
		return;
	}

	// lift button text
	if (TextPaddingSpacer && !bIsIconOnly)
	{
		TextPaddingSpacer->SetSize({1.0f, 4.0f});
	}

	// set size by constraining size box
	if (IconSizeBox)
	{
		SetSizeBox(IconSizeBox, IconSize, IconSize);
	}

	// set the asset
	if (IconAsset && Icon)
	{
		Icon->SetBrushFromTexture(IconAsset, true);
	}

	// update spacer
	if (IconGapSpacer && !bIsIconOnly)
	{
		IconGapSpacer->SetSize({IconGap, 1.0f});
	}

}

void UUICommonButtonBase::SetupUnderline() const
{

	// set visibility
	const ESlateVisibility UnderlineVisibility = ButtonStyle == EUIButtonStyle::Underlined
		                                             ? ESlateVisibility::Visible
		                                             : ESlateVisibility::Collapsed;
	if (UnderlineSizeBox)
	{
		UnderlineSizeBox->SetVisibility(UnderlineVisibility);
	}
	if (UnderlineGapSpacer)
	{
		UnderlineGapSpacer->SetVisibility(UnderlineVisibility);
	}

	if (ButtonStyle != EUIButtonStyle::Underlined)
	{
		return;
	}

	if (UnderlineGapSpacer)
	{
		UnderlineGapSpacer->SetSize({0, UnderlineGap});
	}
	if (UnderlineSizeBox)
	{
		SetSizeBox(UnderlineSizeBox, UnderlineSize);
	}
}

void UUICommonButtonBase::SetupLabel() const
{
	if (ButtonTextWrapper)
	{
		ButtonTextWrapper->SetVisibility(bIsIconOnly ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

void UUICommonButtonBase::ApplyButtonConfiguration()
{
	SetupIcon();
	SetupLabel();
	SetupUnderline();
	SetHoverState();

	switch (ButtonSize)
	{
		case EUIButtonSize::Normal:
			if (NormalButtonStyle)
			{
				SetStyle(NormalButtonStyle);
			}
			break;
		case EUIButtonSize::Small:
			if (SmallButtonStyle)
			{
				SetStyle(SmallButtonStyle);
			}
			break;
	}

	if (ButtonTextBox)
	{
		ButtonTextBox->SetStyle(GetCurrentTextStyleClass());
	}

	if (ButtonBackground)
	{
		if (bTransitionBackground)
		{
			FTransition OneShotTransition = BackgroundTransition;
			OneShotTransition.RepeatTimes = -1;
			OneShotTransition.bYoYo = false;
			BackgroundMotion.Configure(0.0f, 1.0f, OneShotTransition);
			bBackgroundTransitioningIn = false;
			ButtonBackground->SetBrushColor(ButtonBackgroundOn);
			// set the direction of the button background fill -> use pivot point
			// set in X value to 1.0f to transition it right-to-left
			ButtonBackground->SetRenderTransformPivot(FVector2D(0.0f, 0.5f));
			ApplyBackgroundTransition(0.0f);
		}
		else
		{
			ButtonBackground->SetBrushColor(
				bInteractionHighlighted ? ButtonBackgroundOn : ButtonBackgroundOff);
			ButtonBackground->SetRenderOpacity(1.0f);
			ButtonBackground->SetRenderScale(FVector2D(1.0f, 1.0f));
		}
	}
}
