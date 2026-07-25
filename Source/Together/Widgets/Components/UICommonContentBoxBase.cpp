// Uris - All Rights Reserved


#include "Widgets/Components/UICommonContentBoxBase.h"

#include "Brushes/SlateNoResource.h"
#include "Components/BackgroundBlurSlot.h"

void UUICommonContentBoxBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyWidgetSettings();
}

void UUICommonContentBoxBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bInitialSettingsCaptured)
	{
		bInitialCollapsed = bCollapsed;
		bInitialHidden = bHidden;
		bInitialScrolls = bScrolls;
		bInitialSettingsCaptured = true;
	}

	ApplyWidgetSettings();
}

void UUICommonContentBoxBase::ApplyWidgetSettings()
{
	SetHidden(bHidden);
	SetScrolls(bScrolls);

	if (ContentSizeBox)
	{
		ContentSizeBox->SetClipping(EWidgetClipping::ClipToBounds);

		if (Width <= 0.0f && MinWidth <= 0.0f)
		{
			ContentSizeBox->ClearWidthOverride();
			ContentSizeBox->ClearMinDesiredWidth();
			ContentSizeBox->ClearMaxDesiredWidth();
		}
		else
		{
			ContentSizeBox->ClearWidthOverride();
			ContentSizeBox->SetMinDesiredWidth(FMath::Max(0.0f, MinWidth));
			ContentSizeBox->SetMaxDesiredWidth(FMath::Max(Width, MinWidth));
		}

		if (Height <= 0.0f && MinHeight <= 0.0f)
		{
			ContentSizeBox->ClearHeightOverride();
			ContentSizeBox->ClearMinDesiredHeight();

			if (bCollapsed)
			{
				ContentSizeBox->SetMaxDesiredHeight(0.0f);
			}
			else
			{
				ContentSizeBox->ClearMaxDesiredHeight();
			}
		}
		else
		{
			ContentSizeBox->ClearHeightOverride();
			ContentSizeBox->SetMinDesiredHeight(FMath::Max(0.0f, MinHeight));
			ContentSizeBox->SetMaxDesiredHeight(
				bCollapsed ? 0.0f : FMath::Max(Height, MinHeight));
		}
	}

	if (ContentScrollBox)
	{
		FScrollBoxStyle ScrollBoxStyle = ContentScrollBox->GetWidgetStyle();
		const FSlateNoResource NoShadowBrush;
		ScrollBoxStyle.SetTopShadowBrush(NoShadowBrush);
		ScrollBoxStyle.SetBottomShadowBrush(NoShadowBrush);
		ScrollBoxStyle.SetLeftShadowBrush(NoShadowBrush);
		ScrollBoxStyle.SetRightShadowBrush(NoShadowBrush);
		ContentScrollBox->SetWidgetStyle(ScrollBoxStyle);

		FScrollBarStyle ScrollBarStyle = ContentScrollBox->GetWidgetBarStyle();
		ScrollBarStyle.SetHorizontalBackgroundImage(NoShadowBrush);
		ScrollBarStyle.SetVerticalBackgroundImage(NoShadowBrush);
		ScrollBarStyle.SetHorizontalTopSlotImage(NoShadowBrush);
		ScrollBarStyle.SetHorizontalBottomSlotImage(NoShadowBrush);
		ScrollBarStyle.SetVerticalTopSlotImage(NoShadowBrush);
		ScrollBarStyle.SetVerticalBottomSlotImage(NoShadowBrush);
		ContentScrollBox->SetWidgetBarStyle(ScrollBarStyle);

		if (UBackgroundBlurSlot* BackgroundSlot = Cast<UBackgroundBlurSlot>(ContentScrollBox->Slot))
		{
			BackgroundSlot->SetPadding(ContentInnerPadding);
			BackgroundSlot->SetHorizontalAlignment(ContentHorizontalJustification);
			BackgroundSlot->SetVerticalAlignment(ContentVerticalJustification);
		}
	}

	if (ContentBackground)
	{
		ContentBackground->SetBlurStrength(BackgroundBlurStrength);
	}

	if (ContentBorder)
	{
		ContentBorder->SetBrushColor(BackgroundColor);
	}

	if (TopBorderSize)
	{
		TopBorderSize->SetHeightOverride(BorderSize);
		TopBorderSize->SetMinDesiredHeight(BorderSize);
		TopBorderSize->SetMaxDesiredHeight(BorderSize);
	}

	if (BottomBorderSize)
	{
		BottomBorderSize->SetHeightOverride(BorderSize);
		BottomBorderSize->SetMinDesiredHeight(BorderSize);
		BottomBorderSize->SetMaxDesiredHeight(BorderSize);
	}

	if (TopBorder)
	{
		TopBorder->SetBrushColor(BorderColor);
	}

	if (BottomBorder)
	{
		BottomBorder->SetBrushColor(BorderColor);
	}

	SetBorderOpacity(bCollapsed ? 0.0f : 1.0f);
	SetBorderVisibility(
		bCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UUICommonContentBoxBase::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bCollapseTransitionActive || !ContentSizeBox)
	{
		return;
	}

	CollapseTransitionElapsed += InDeltaTime;
	const float Delay = FMath::Max(0.0f, Transition.Delay);

	if (CollapseTransitionElapsed < Delay)
	{
		return;
	}

	const float Duration = FMath::Max(0.0f, Transition.Duration);
	if (Duration <= KINDA_SMALL_NUMBER)
	{
		if (bAnimateHeightDuringTransition)
		{
			ContentSizeBox->SetMaxDesiredHeight(CollapseTransitionTargetHeight);
		}
		SetBorderOpacity(BorderTransitionTargetOpacity);
		SetBorderVisibility(
			bCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		bCollapseTransitionActive = false;
		OnTransitionEnd.Broadcast(bCollapsed);
		return;
	}

	const float Progress = FMath::Clamp((CollapseTransitionElapsed - Delay) / Duration, 0.0f, 1.0f);
	const float EasedAlpha = EvaluateTransitionAlpha(Progress);
	if (bAnimateHeightDuringTransition)
	{
		const float CurrentHeight = FMath::Lerp(
			CollapseTransitionStartHeight,
			CollapseTransitionTargetHeight,
			EasedAlpha);

		ContentSizeBox->SetMaxDesiredHeight(CurrentHeight);
	}
	SetBorderOpacity(FMath::Lerp(
		BorderTransitionStartOpacity,
		BorderTransitionTargetOpacity,
		EasedAlpha));

	if (Progress >= 1.0f)
	{
		if (bAnimateHeightDuringTransition)
		{
			ContentSizeBox->SetMaxDesiredHeight(CollapseTransitionTargetHeight);
		}
		SetBorderOpacity(BorderTransitionTargetOpacity);
		SetBorderVisibility(
			bCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		bCollapseTransitionActive = false;
		OnTransitionEnd.Broadcast(bCollapsed);
	}
}

void UUICommonContentBoxBase::SetCollapsed(const bool bCollapse, const bool bImmediately)
{
	// An active transition already moving toward this state does not need to be restarted.
	// Restarting it would reset its elapsed time and make repeated requests delay completion.
	if (bCollapseTransitionActive && bCollapseTransitionTarget == bCollapse && !bImmediately)
	{
		return;
	}

	if (bCollapseTransitionActive)
	{
		// Interrupt the current transition without broadcasting OnTransitionEnd. The previous
		// destination was never reached, so reporting completion could trigger stale Blueprint
		// callbacks or deactivation logic. PerformCollapseTransition will capture the current
		// height and opacity, allowing an opposite request to reverse smoothly from this point.
		bCollapseTransitionActive = false;
	}

	bCollapsed = bCollapse;

	if (bImmediately)
	{
		bCollapseTransitionActive = false;
		bAnimateHeightDuringTransition = false;

		if (ContentSizeBox)
		{
			if (bCollapsed)
			{
				ContentSizeBox->SetMaxDesiredHeight(0.0f);
			}
			else if (Height <= 0.0f && MinHeight <= 0.0f)
			{
				ContentSizeBox->ClearMaxDesiredHeight();
			}
			else
			{
				ContentSizeBox->SetMaxDesiredHeight(FMath::Max(Height, MinHeight));
			}
		}

		SetBorderOpacity(bCollapsed ? 0.0f : 1.0f);
		SetBorderVisibility(
			bCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		return;
	}

	PerformCollapseTransition(bCollapsed);
}

void UUICommonContentBoxBase::PerformCollapseTransition(const bool bCollapse)
{
	if (!ContentSizeBox)
	{
		bCollapseTransitionActive = false;
		return;
	}

	// Record the requested destination before broadcasting transition start. This lets
	// SetCollapsed distinguish duplicate requests from requests that reverse direction.
	bCollapseTransitionTarget = bCollapse;
	OnTransitionStart.Broadcast(bCollapse);

	CollapseTransitionStartHeight = ContentSizeBox->GetMaxDesiredHeight();
	BorderTransitionStartOpacity = TopBorderSize
		                               ? TopBorderSize->GetRenderOpacity()
		                               : (bCollapse ? 1.0f : 0.0f);
	BorderTransitionTargetOpacity = bCollapse ? 0.0f : 1.0f;
	SetBorderVisibility(ESlateVisibility::SelfHitTestInvisible);
	bAnimateHeightDuringTransition = true;
	CollapseTransitionElapsed = 0.0f;

	if (!bCollapse && Height <= 0.0f && MinHeight <= 0.0f)
	{
		ContentSizeBox->ClearMaxDesiredHeight();
		bAnimateHeightDuringTransition = false;

		if (Transition.Delay <= 0.0f && Transition.Duration <= KINDA_SMALL_NUMBER)
		{
			SetBorderOpacity(1.0f);
			bCollapseTransitionActive = false;
			OnTransitionEnd.Broadcast(bCollapse);
			return;
		}

		bCollapseTransitionActive = true;
		return;
	}

	CollapseTransitionTargetHeight = bCollapse ? 0.0f : FMath::Max(Height, MinHeight);

	if (Transition.Delay <= 0.0f && Transition.Duration <= KINDA_SMALL_NUMBER)
	{
		ContentSizeBox->SetMaxDesiredHeight(CollapseTransitionTargetHeight);
		SetBorderOpacity(BorderTransitionTargetOpacity);
		SetBorderVisibility(
			bCollapse ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		bCollapseTransitionActive = false;
		OnTransitionEnd.Broadcast(bCollapse);
		return;
	}

	bCollapseTransitionActive = true;
}

void UUICommonContentBoxBase::SetBorderVisibility(const ESlateVisibility InVisibility) const
{
	if (TopBorderSize)
	{
		TopBorderSize->SetVisibility(InVisibility);
	}

	if (BottomBorderSize)
	{
		BottomBorderSize->SetVisibility(InVisibility);
	}
}

void UUICommonContentBoxBase::SetBorderOpacity(const float Opacity) const
{
	if (TopBorderSize)
	{
		TopBorderSize->SetRenderOpacity(Opacity);
	}

	if (BottomBorderSize)
	{
		BottomBorderSize->SetRenderOpacity(Opacity);
	}
}

float UUICommonContentBoxBase::EvaluateTransitionAlpha(const float Alpha) const
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	switch (Transition.TransitionType)
	{
		case ECustomTransitionType::EaseInOut:
			return FMath::InterpEaseInOut(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::EaseIn:
			return FMath::InterpEaseIn(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::EaseOut:
			return FMath::InterpEaseOut(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::Curve:
			return Transition.Curve
				       ? FMath::Clamp(Transition.Curve->GetFloatValue(ClampedAlpha), 0.0f, 1.0f)
				       : ClampedAlpha;

		case ECustomTransitionType::Linear:
		default:
			return ClampedAlpha;
	}
}

void UUICommonContentBoxBase::SetHidden(const bool bHide)
{
	bHidden = bHide;
	SetVisibility(bHidden ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UUICommonContentBoxBase::SetScrolls(const bool bEnableScrolling)
{
	bScrolls = bEnableScrolling;

	if (!ContentScrollBox)
	{
		return;
	}

	ContentScrollBox->SetScrollBarVisibility(
		bScrolls ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ContentScrollBox->SetConsumeMouseWheel(
		bScrolls ? EConsumeMouseWheel::WhenScrollingPossible : EConsumeMouseWheel::Never);
	ContentScrollBox->SetIsTouchScrollingEnabled(bScrolls);
	ContentScrollBox->SetAllowRightClickDragScrolling(bScrolls);
	ContentScrollBox->SetAllowOverscroll(bScrolls);
	ContentScrollBox->SetScrollWhenFocusChanges(EScrollWhenFocusChanges::NoScroll);

	if (!bScrolls)
	{
		ContentScrollBox->SetScrollOffset(0.0f);
	}
}

void UUICommonContentBoxBase::SetVerticalScrollOffset(const float Offset) const
{
	if (ContentScrollBox && Offset >= 0.0f)
	{
		ContentScrollBox->SetScrollOffset(Offset);
	}
}

void UUICommonContentBoxBase::RestoreInitialSettings()
{
	if (!bInitialSettingsCaptured)
	{
		return;
	}

	bCollapseTransitionActive = false;
	bAnimateHeightDuringTransition = false;
	CollapseTransitionElapsed = 0.0f;
	bCollapseTransitionTarget = bInitialCollapsed;

	bCollapsed = bInitialCollapsed;
	bHidden = bInitialHidden;
	bScrolls = bInitialScrolls;

	ApplyWidgetSettings();
}
