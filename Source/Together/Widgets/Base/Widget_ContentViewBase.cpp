// Uris - All Rights Reserved


#include "Widget_ContentViewBase.h"

#include "TimerManager.h"
#include "Widgets/Components/UICommonContentBoxBase.h"
#include "Components/VerticalBox.h"

void UWidget_ContentViewBase::NativeConstruct()
{

	Super::NativeConstruct();

	// designate delegate for transition end
	if (ContentView)
	{
		ContentView->OnTransitionStart.AddUniqueDynamic(
			this,
			&UWidget_ContentViewBase::HandleContentBoxTransitionStart);
		ContentView->OnTransitionEnd.AddUniqueDynamic(this, &UWidget_ContentViewBase::HandleContentBoxCollapse);
	}

	// init header/footer state in prep for initial transition
	if (!bContentChromeOpacityInitialized)
	{
		ContentViewHeaderOpacity = ContentViewHeader ? ContentViewHeader->GetRenderOpacity() : 1.0f;
		ContentViewFooterOpacity = ContentViewFooter ? ContentViewFooter->GetRenderOpacity() : 1.0f;
		bContentChromeOpacityInitialized = true;
	}

}

void UWidget_ContentViewBase::NativeDestruct()
{

	// release delegate for transition end
	if (ContentView)
	{
		ContentView->OnTransitionStart.RemoveDynamic(
			this,
			&UWidget_ContentViewBase::HandleContentBoxTransitionStart);
		ContentView->OnTransitionEnd.RemoveDynamic(this, &UWidget_ContentViewBase::HandleContentBoxCollapse);
	}

	// releaser timer handle
	ClearFadeTimer();

	// reset state
	bForceContentChromeFade = false;

	Super::NativeDestruct();
}

void UWidget_ContentViewBase::NativeOnActivated()
{
	Super::NativeOnActivated();

	// clear click delay and fade out timers
	ClearFadeTimer();

	// fade is not forced and will be a function of the setting
	bForceContentChromeFade = false;

	// sync chrome fade in/out value to the content view
	SetContentChromeFadeAlpha(ContentView && ContentView->bCollapsed ? 0.0f : 1.0f);
}

void UWidget_ContentViewBase::NativeOnDeactivated()
{
	// clear click delay and fade out timers
	ClearFadeTimer();

	// fade is not forced and will be a function of the setting
	bForceContentChromeFade = false;

	// sync chrome fade in/out value to the content view
	SetContentChromeFadeAlpha(ContentView && ContentView->bCollapsed ? 0.0f : 1.0f);

	Super::NativeOnDeactivated();
}

bool UWidget_ContentViewBase::NativeOnHandleBackAction()
{
	if (IsExitPending())
	{
		return true;
	}

	return RequestExit(EWidgetExitType::Back);
}

bool UWidget_ContentViewBase::BeginExitTransition(EWidgetExitType Type)
{
	// protect for content view and that it should collapse before pop
	if (!bViewCollapsesBeforePop || !ContentView)
	{
		return false;
	}

	// no transition is necessary
	if (ContentView->bCollapsed)
	{
		return false;
	}

	// start transition
	ContentView->SetCollapsed(true);

	return true;
}

void UWidget_ContentViewBase::HandleContentBoxCollapse(const bool bIsCollapsed)
{
	if (!bIsCollapsed)
	{
		return;
	}

	ClearFadeTimer();
	SetContentChromeFadeAlpha(0.0f);

	if (IsExitPending())
	{
		CompleteExit();
	}
}

void UWidget_ContentViewBase::HandleContentBoxTransitionStart(const bool bIsCollapsed)
{
	// A collapse initiated by the delayed Back path is an exit transition. Always fade
	// both chrome regions for that exit, even if normal header/footer fading is disabled.
	bForceContentChromeFade =
		bIsCollapsed && IsExitPending() && bViewCollapsesBeforePop;

	// Every content transition drives the optional chrome: fade out while collapsing
	// and fade in while expanding, regardless of what initiated the transition.
	StartContentChromeFade(bIsCollapsed);
}

void UWidget_ContentViewBase::SetHeaderShouldFade(const bool bShouldFade)
{
	bHeaderShouldFade = bShouldFade;

	// A header excluded from transitions must remain at its authored opacity.
	if (!bHeaderShouldFade && !bForceContentChromeFade && ContentViewHeader)
	{
		ContentViewHeader->SetRenderOpacity(ContentViewHeaderOpacity);
	}
}

void UWidget_ContentViewBase::SetFooterShouldFade(const bool bShouldFade)
{
	bFooterShouldFade = bShouldFade;

	// A footer excluded from transitions must remain at its authored opacity.
	if (!bFooterShouldFade && !bForceContentChromeFade && ContentViewFooter)
	{
		ContentViewFooter->SetRenderOpacity(ContentViewFooterOpacity);
	}
}

void UWidget_ContentViewBase::StartContentChromeFade(const bool bFadeOut)
{
	// Begin from the current visual opacity so interrupted/reversed transitions remain smooth.
	const float DefaultStartAlpha = bFadeOut ? 1.0f : 0.0f;
	ContentHeaderFadeStartAlpha =
		ContentViewHeader && ContentViewHeaderOpacity > KINDA_SMALL_NUMBER
			? ContentViewHeader->GetRenderOpacity() / ContentViewHeaderOpacity
			: DefaultStartAlpha;
	ContentFooterFadeStartAlpha =
		ContentViewFooter && ContentViewFooterOpacity > KINDA_SMALL_NUMBER
			? ContentViewFooter->GetRenderOpacity() / ContentViewFooterOpacity
			: DefaultStartAlpha;

	ContentHeaderFadeStartAlpha = FMath::Clamp(ContentHeaderFadeStartAlpha, 0.0f, 1.0f);
	ContentFooterFadeStartAlpha = FMath::Clamp(ContentFooterFadeStartAlpha, 0.0f, 1.0f);
	ContentChromeFadeTargetAlpha = bFadeOut ? 0.0f : 1.0f;

	UWorld* World = GetWorld();
	if (!World || !ContentView)
	{
		SetContentChromeFadeAlpha(ContentChromeFadeTargetAlpha);
		return;
	}

	World->GetTimerManager().ClearTimer(ContentChromeFadeTimerHandle);
	ContentChromeFadeStartTime = World->GetTimeSeconds();
	UpdateContentChromeFade();

	const float TotalDuration = FMath::Max(0.0f, ContentView->Transition.Delay)
	                            + FMath::Max(0.0f, ContentView->Transition.Duration);
	if (TotalDuration > KINDA_SMALL_NUMBER)
	{
		World->GetTimerManager().SetTimer(
			ContentChromeFadeTimerHandle,
			this,
			&ThisClass::UpdateContentChromeFade,
			1.0f / 60.0f,
			true);
	}
	else
	{
		SetContentChromeFadeAlpha(ContentChromeFadeTargetAlpha);
	}
}

void UWidget_ContentViewBase::UpdateContentChromeFade()
{
	UWorld* World = GetWorld();
	if (!World || !ContentView)
	{
		return;
	}

	const float Elapsed = World->GetTimeSeconds() - ContentChromeFadeStartTime;
	const float Delay = FMath::Max(0.0f, ContentView->Transition.Delay);
	if (Elapsed < Delay)
	{
		return;
	}

	const float Duration = FMath::Max(0.0f, ContentView->Transition.Duration);
	const float Progress = Duration <= KINDA_SMALL_NUMBER
		                       ? 1.0f
		                       : FMath::Clamp((Elapsed - Delay) / Duration, 0.0f, 1.0f);

	const float EasedAlpha = EvaluateContentChromeFadeAlpha(Progress);
	const float HeaderAlpha = FMath::Lerp(
		ContentHeaderFadeStartAlpha,
		ContentChromeFadeTargetAlpha,
		EasedAlpha);
	const float FooterAlpha = FMath::Lerp(
		ContentFooterFadeStartAlpha,
		ContentChromeFadeTargetAlpha,
		EasedAlpha);

	if (ContentViewHeader)
	{
		ContentViewHeader->SetRenderOpacity(
			ContentViewHeaderOpacity *
			((bHeaderShouldFade || bForceContentChromeFade) ? HeaderAlpha : 1.0f));
	}

	if (ContentViewFooter)
	{
		ContentViewFooter->SetRenderOpacity(
			ContentViewFooterOpacity *
			((bFooterShouldFade || bForceContentChromeFade) ? FooterAlpha : 1.0f));
	}

	if (Progress >= 1.0f)
	{
		SetContentChromeFadeAlpha(ContentChromeFadeTargetAlpha);
		World->GetTimerManager().ClearTimer(ContentChromeFadeTimerHandle);
	}
}

void UWidget_ContentViewBase::SetContentChromeFadeAlpha(const float Alpha) const
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	if (ContentViewHeader)
	{
		ContentViewHeader->SetRenderOpacity(
			ContentViewHeaderOpacity *
			((bHeaderShouldFade || bForceContentChromeFade) ? ClampedAlpha : 1.0f));
	}

	if (ContentViewFooter)
	{
		ContentViewFooter->SetRenderOpacity(
			ContentViewFooterOpacity *
			((bFooterShouldFade || bForceContentChromeFade) ? ClampedAlpha : 1.0f));
	}
}

float UWidget_ContentViewBase::EvaluateContentChromeFadeAlpha(const float Alpha) const
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	if (!ContentView)
	{
		return ClampedAlpha;
	}

	switch (ContentView->Transition.TransitionType)
	{
		case ECustomTransitionType::EaseInOut:
			return FMath::InterpEaseInOut(0.0f, 1.0f, ClampedAlpha, 2.0f);
		case ECustomTransitionType::EaseIn:
			return FMath::InterpEaseIn(0.0f, 1.0f, ClampedAlpha, 2.0f);
		case ECustomTransitionType::EaseOut:
			return FMath::InterpEaseOut(0.0f, 1.0f, ClampedAlpha, 2.0f);
		case ECustomTransitionType::Curve:
			return ContentView->Transition.Curve
				       ? FMath::Clamp(ContentView->Transition.Curve->GetFloatValue(ClampedAlpha), 0.0f, 1.0f)
				       : ClampedAlpha;
		case ECustomTransitionType::Linear:
		default:
			return ClampedAlpha;
	}
}

float UWidget_ContentViewBase::GetViewHeight() const
{
	if (ContentView)
	{
		return ContentView->Height;
	}

	return 0.0f;
}

void UWidget_ContentViewBase::ClearFadeTimer()
{
	if (const UWorld* World = GetWorld(); World && ContentChromeFadeTimerHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(ContentChromeFadeTimerHandle);
	}
}