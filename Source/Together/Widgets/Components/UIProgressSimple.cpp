// Uris - All Rights Reserved

#include "Widgets/Components/UIProgressSimple.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"

void UUIProgressSimple::NativePreConstruct()
{
	Super::NativePreConstruct();
	ApplyConfiguration();
}

void UUIProgressSimple::NativeConstruct()
{
	Super::NativeConstruct();
	ApplyConfiguration();

	if (bAutoStart)
	{
		Start();
	}
	else
	{
		Stop();
	}
}

void UUIProgressSimple::NativeDestruct()
{
	Stop();
	Super::NativeDestruct();
}

void UUIProgressSimple::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!BarSizeBox || !BarMotion.IsPlaying())
	{
		return;
	}

	const bool bValueChanged = BarMotion.Tick(InDeltaTime);
	if (bValueChanged)
	{
		SetBarPosition(BarMotion.GetValue());
	}

	if (BarMotion.DidCycleStart())
	{
		OnTransitionStart.Broadcast();
	}

	if (BarMotion.DidComplete())
	{
		OnTransitionEnd.Broadcast();
	}
}

void UUIProgressSimple::Start()
{
	BarMotion.Configure(-Width, Width, Transition);
	BarMotion.Play();
	SetBarPosition(BarMotion.GetValue());
}

void UUIProgressSimple::Stop()
{
	BarMotion.Stop(true);
	SetBarPosition(BarMotion.GetValue());
}

void UUIProgressSimple::ApplyConfiguration()
{
	Width = FMath::Max(0.0f, Width);
	Height = FMath::Max(0.0f, Height);

	if (TrackSizeBox)
	{
		TrackSizeBox->SetWidthOverride(Width);
		TrackSizeBox->SetHeightOverride(Height);
	}

	if (BarSizeBox)
	{
		BarSizeBox->SetWidthOverride(Width);
		BarSizeBox->SetHeightOverride(Height);

		if (UCanvasPanelSlot* BarSlot = Cast<UCanvasPanelSlot>(BarSizeBox->Slot))
		{
			BarSlot->SetAutoSize(false);
			BarSlot->SetSize(FVector2D(Width, Height));
			BarSlot->SetAlignment(FVector2D::ZeroVector);
		}
	}

	if (TrackBorder)
	{
		TrackBorder->SetBrushColor(TrackColor);
		TrackBorder->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}

	if (ProgressCanvas)
	{
		ProgressCanvas->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}

	if (BarBorder)
	{
		BarBorder->SetBrushColor(BarColor);
	}

	const bool bWasPlaying = BarMotion.IsPlaying();
	BarMotion.Configure(-Width, Width, Transition);
	if (bWasPlaying)
	{
		BarMotion.Play();
	}
	SetBarPosition(BarMotion.GetValue());
}

void UUIProgressSimple::SetBarPosition(const float X) const
{
	if (BarSizeBox)
	{
		if (UCanvasPanelSlot* BarSlot = Cast<UCanvasPanelSlot>(BarSizeBox->Slot))
		{
			BarSlot->SetPosition(FVector2D(X, 0.0f));
		}
	}
}
