// Uris - All Rights Reserved


#include "Widgets/Components/UICommonProgressText.h"

void UUICommonProgressText::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetVisibility(bIsCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();

	if (TextBlock && !bIsWorking)
	{
		TextBlock->SetRenderOpacity(1.0f);
	}
}

void UUICommonProgressText::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SetVisibility(bIsCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();

	// Designer previews can retain the last animated opacity. Idle text must
	// always be visible when this widget is placed inside another widget.
	if (TextBlock && !bIsWorking)
	{
		TextBlock->SetRenderOpacity(1.0f);
	}
}

void UUICommonProgressText::NativeConstruct()
{
	Super::NativeConstruct();

	if (!bInitialValuesCaptured)
	{
		bInitialWorking = bIsWorking;
		bInitialCollapsed = bIsCollapsed;
		InitialTransition = Transition;
		InitialStartingOpacity = StartingOpacity;
		InitialEndingOpacity = EndingOpacity;
		InitialDefaultText = DefaultText;
		InitialWorkingText = WorkingText;
		InitialDefaultTextStyle = DefaultTextStyle;
		InitialWorkingTextStyle = WorkingTextStyle;
		bInitialValuesCaptured = true;
	}

	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();

	if (bIsWorking && !bIsCollapsed)
	{
		StartWorkingTransition();
	}
}

void UUICommonProgressText::NativeDestruct()
{
	StopWorkingTransition();
	Super::NativeDestruct();
}

void UUICommonProgressText::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!TextBlock || !OpacityMotion.IsPlaying())
	{
		return;
	}

	if (OpacityMotion.Tick(InDeltaTime))
	{
		TextBlock->SetRenderOpacity(OpacityMotion.GetValue());
	}

	if (OpacityMotion.DidComplete())
	{
		OnTransitionEnd.Broadcast(bIsWorking);
	}
}

void UUICommonProgressText::SetWorking(FString Text)
{
	if (!Text.IsEmpty())
	{
		SetProgressText(MoveTemp(Text));
	}

	SetCollapsed(false);
	SetProgress(true);
}

void UUICommonProgressText::SetIdle(FString Text)
{
	if (!Text.IsEmpty())
	{
		SetDefaultText(MoveTemp(Text));
	}

	SetCollapsed(false);
	SetProgress(false);
}

void UUICommonProgressText::SetProgress(const bool bWorking)
{
	if (bIsWorking == bWorking)
	{
		return;
	}

	bIsWorking = bWorking;
	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();

	if (bIsWorking && !bIsCollapsed)
	{
		StartWorkingTransition();
	}
	else
	{
		StopWorkingTransition();
	}
}

void UUICommonProgressText::SetCollapsed(const bool bCollapse)
{
	bIsCollapsed = bCollapse;

	if (bIsCollapsed)
	{
		// Reset before hiding so the next reveal never inherits a partial fade.
		StopWorkingTransition();
		SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();

	if (bIsWorking)
	{
		StartWorkingTransition();
	}
	else
	{
		StopWorkingTransition();
	}
}

void UUICommonProgressText::Reset()
{
	StopWorkingTransition();

	if (bInitialValuesCaptured)
	{
		Transition = InitialTransition;
		StartingOpacity = InitialStartingOpacity;
		EndingOpacity = InitialEndingOpacity;
		DefaultText = InitialDefaultText;
		WorkingText = InitialWorkingText;
		DefaultTextStyle = InitialDefaultTextStyle;
		WorkingTextStyle = InitialWorkingTextStyle;
		bIsWorking = bInitialWorking;
		bIsCollapsed = bInitialCollapsed;
	}

	CurrentText = bIsWorking ? WorkingText : DefaultText;
	ApplyTextValue();
	ApplyTextStyle();
	SetVisibility(bIsCollapsed ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

	if (bIsWorking && !bIsCollapsed)
	{
		StartWorkingTransition();
	}
}

void UUICommonProgressText::SetDefaultText(FString Text)
{
	DefaultText = MoveTemp(Text);
	if (!bIsWorking)
	{
		CurrentText = DefaultText;
		ApplyTextValue();
	}
}

void UUICommonProgressText::SetProgressText(FString Text)
{
	WorkingText = MoveTemp(Text);
	if (bIsWorking)
	{
		CurrentText = WorkingText;
		ApplyTextValue();
	}
}

void UUICommonProgressText::ApplyTextStyle() const
{
	if (bIsWorking && WorkingTextStyle && TextBlock)
	{
		TextBlock->SetStyle(WorkingTextStyle);
	}
	else if (!bIsWorking && DefaultTextStyle && TextBlock)
	{
		TextBlock->SetStyle(DefaultTextStyle);
	}
}

void UUICommonProgressText::ApplyTextValue() const
{
	if (TextBlock)
	{
		TextBlock->SetText(FText::FromString(CurrentText));
	}
}

void UUICommonProgressText::StartWorkingTransition()
{
	const bool bWasTransitionActive = OpacityMotion.IsPlaying();
	OpacityMotion.Configure(
		FMath::Clamp(StartingOpacity, 0.0f, 1.0f),
		FMath::Clamp(EndingOpacity, 0.0f, 1.0f),
		Transition);

	if (TextBlock)
	{
		OpacityMotion.Play();
		TextBlock->SetRenderOpacity(OpacityMotion.GetValue());
	}

	if (OpacityMotion.IsPlaying() && !bWasTransitionActive)
	{
		OnTransitionStart.Broadcast(bIsWorking);
	}
}

void UUICommonProgressText::StopWorkingTransition()
{
	const bool bWasTransitionActive = OpacityMotion.IsPlaying();
	OpacityMotion.Stop(true);

	if (TextBlock)
	{
		TextBlock->SetRenderOpacity(OpacityMotion.GetValue());
	}

	if (bWasTransitionActive)
	{
		OnTransitionEnd.Broadcast(bIsWorking);
	}
}
