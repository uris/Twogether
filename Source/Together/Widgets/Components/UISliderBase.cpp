// Uris - All Rights Reserved


#include "UISliderBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Widgets/Options/ListView/ListEntryStyle.h"
#include "Widgets/Options/ListView/UIOptionsListEntry.h"

void UUISliderBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!Slider)
	{
		return;
	}

	FSliderStyle SliderStyle = Slider->GetWidgetStyle();
	SliderStyle.SetBarThickness(FMath::Max(0.0f, TrackHeight));

	// config for slider track
	auto ConfigureTrackBrush = [this](FSlateBrush Brush)
	{
		Brush.TintColor = FSlateColor(TrackColor);
		return Brush;
	};

	SliderStyle.SetNormalBarImage(ConfigureTrackBrush(SliderStyle.NormalBarImage));
	SliderStyle.SetHoveredBarImage(ConfigureTrackBrush(SliderStyle.HoveredBarImage));
	SliderStyle.SetDisabledBarImage(ConfigureTrackBrush(SliderStyle.DisabledBarImage));

	// config for the slider handle
	auto ConfigureThumbBrush = [this](FSlateBrush Brush)
	{
		Brush.ImageSize = FVector2D(HandleSize);
		Brush.TintColor = FSlateColor(FLinearColor::White);
		return Brush;
	};

	SliderStyle.SetNormalThumbImage(ConfigureThumbBrush(SliderStyle.NormalThumbImage));
	SliderStyle.SetHoveredThumbImage(ConfigureThumbBrush(SliderStyle.HoveredThumbImage));
	SliderStyle.SetDisabledThumbImage(ConfigureThumbBrush(SliderStyle.DisabledThumbImage));

	Slider->SetWidgetStyle(SliderStyle);
	if (SliderValue)
	{
		SliderValue->SetVisibility(bShowValue ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	if (Progress)
	{
		if (UCanvasPanelSlot* ProgressCanvasSlot = Cast<UCanvasPanelSlot>(Progress->Slot))
		{
			ProgressCanvasSlot->SetSize(FVector2D(ProgressCanvasSlot->GetSize().X, TrackHeight));
		}
		Progress->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (SliderValueSizeBox)
	{
		SliderValueSizeBox->SetMinDesiredWidth(ValueBoxSize);
		SliderValueSizeBox->SetMaxDesiredWidth(ValueBoxSize);
		SliderValueSizeBox->SetWidthOverride(ValueBoxSize);
	}

	if (SliderSizeBox)
	{
		SliderSizeBox->SetMinDesiredHeight(FMath::Max(SliderHitAreaHeight, HandleSize));
	}

	UpdateSliderStyle(FListEntryState(false, false, GetIsEnabled()));
}

void UUISliderBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider)
	{
		Slider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleSliderValueChanged);
	}

	UpdateSliderStyle(FListEntryState(false, false, GetIsEnabled()));
	ApplyProgress();
}

void UUISliderBase::NativeDestruct()
{
	if (Slider)
	{
		Slider->OnValueChanged.RemoveDynamic(this, &ThisClass::HandleSliderValueChanged);
	}

	Super::NativeDestruct();
}

void UUISliderBase::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!Slider)
	{
		return;
	}

	const float SliderWidth = Slider->GetCachedGeometry().GetLocalSize().X;
	if (!FMath::IsNearlyEqual(SliderWidth, CurrentSliderWidth))
	{
		CurrentSliderWidth = SliderWidth;
		ApplyProgress();
	}
}

void UUISliderBase::ApplyProgress() const
{
	if (!Slider || !Progress)
	{
		return;
	}

	UCanvasPanelSlot* ProgressSlot = Cast<UCanvasPanelSlot>(Progress->Slot);
	if (!ProgressSlot)
	{
		return;
	}

	const float SliderWidth = Slider->GetCachedGeometry().GetLocalSize().X;
	if (SliderWidth <= 0.0f)
	{
		return;
	}

	const float MinValue = Slider->GetMinValue();
	const float MaxValue = Slider->GetMaxValue();
	const float Percent = FMath::IsNearlyEqual(MinValue, MaxValue)
		                      ? 0.0f
		                      : FMath::Clamp(
			                      (Slider->GetValue() - MinValue) / (MaxValue - MinValue),
			                      0.0f,
			                      1.0f);

	FMargin Offsets = ProgressSlot->GetOffsets();
	Offsets.Right = SliderWidth * (1.0f - Percent);
	ProgressSlot->SetOffsets(Offsets);
}

void UUISliderBase::HandleSliderValueChanged(const float) const
{
	ApplyProgress();
}

void UUISliderBase::UpdateSliderStyle(const FListEntryState EntryState) const
{
	FLinearColor CurrentBarColor = BarColor;
	FLinearColor CurrentHandleColor = HandleColor;
	TSubclassOf<UCommonTextStyle> CurrentValueTextStyle;

	if (!GetIsEnabled() && SliderValue && ValueDisabledTextStyle)
	{
		CurrentValueTextStyle = ValueDisabledTextStyle;
	}

	else if (EntryState.bSelected)
	{
		CurrentBarColor = BarColorSelected;
		CurrentHandleColor = HandleColorSelected;
		CurrentValueTextStyle = ValueSelectedTextStyle ? ValueSelectedTextStyle : ValueDefaultTextStyle;
	}
	else if (EntryState.bHovered)
	{
		CurrentBarColor = BarColorHovered;
		CurrentHandleColor = HandleColorHovered;
		CurrentValueTextStyle = ValueHoveredTextStyle ? ValueHoveredTextStyle : ValueDefaultTextStyle;
	}
	else
	{
		CurrentBarColor = BarColor;
		CurrentHandleColor = HandleColor;
		CurrentValueTextStyle = ValueDefaultTextStyle;
	}

	if (Slider)
	{
		Slider->SetSliderBarColor(CurrentBarColor);
		Slider->SetSliderHandleColor(CurrentHandleColor);
	}

	if (Progress)
	{
		Progress->SetColorAndOpacity(CurrentBarColor);
	}

	if (SliderValue && CurrentValueTextStyle)
	{
		SliderValue->SetStyle(CurrentValueTextStyle);
	}
}

void UUISliderBase::UpdateTextStyles(const FListTextStyle& InTextStyles)
{
	if (InTextStyles.DefaultTextStyle)
	{
		ValueDefaultTextStyle = InTextStyles.DefaultTextStyle;
	}
	if (InTextStyles.HoveredTextStyle)
	{
		ValueHoveredTextStyle = InTextStyles.HoveredTextStyle;
		ValueSelectedTextStyle = InTextStyles.HoveredTextStyle;
	}
	if (InTextStyles.DisabledTextStyle)
	{
		ValueDisabledTextStyle = InTextStyles.DisabledTextStyle;
	}
}
