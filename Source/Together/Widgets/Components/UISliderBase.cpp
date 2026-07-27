// Uris - All Rights Reserved


#include "UISliderBase.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UUISliderBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!Slider)
	{
		return;
	}

	FSliderStyle SliderStyle = Slider->GetWidgetStyle();
	SliderStyle.SetBarThickness(FMath::Max(0.0f, TrackHeight));

	auto ConfigureBarBrush = [this](FSlateBrush Brush)
	{
		Brush.TintColor = FSlateColor(TrackColor);
		return Brush;
	};

	SliderStyle.SetNormalBarImage(ConfigureBarBrush(SliderStyle.NormalBarImage));
	SliderStyle.SetHoveredBarImage(ConfigureBarBrush(SliderStyle.HoveredBarImage));
	SliderStyle.SetDisabledBarImage(ConfigureBarBrush(SliderStyle.DisabledBarImage));

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
		Progress->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (SliderValueSizeBox)
	{
		SliderValueSizeBox->SetMinDesiredWidth(ValueBoxSize);
		SliderValueSizeBox->SetMaxDesiredWidth(ValueBoxSize);
		SliderValueSizeBox->SetWidthOverride(ValueBoxSize);
	}

	UpdateSliderStyle(false, false);
}

void UUISliderBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (Slider)
	{
		Slider->OnValueChanged.AddUniqueDynamic(this, &ThisClass::HandleSliderValueChanged);
	}

	UpdateSliderStyle(false, false);
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

void UUISliderBase::UpdateSliderStyle(const bool bIsSelected, const bool bIsHovered) const
{
	FLinearColor CurrentBarColor = BarColor;
	FLinearColor CurrentHandleColor = HandleColor;
	TSubclassOf<UCommonTextStyle> CurrentValueTextStyle = ValueDefaultTextStyle;

	if (bIsSelected)
	{
		CurrentBarColor = BarColorSelected;
		CurrentHandleColor = HandleColorSelected;
		CurrentValueTextStyle = ValueSelectedTextStyle
			                        ? ValueSelectedTextStyle
			                        : ValueDefaultTextStyle;
	}
	else if (bIsHovered)
	{
		CurrentBarColor = BarColorHovered;
		CurrentHandleColor = HandleColorHovered;
		CurrentValueTextStyle = ValueHoveredTextStyle
			                        ? ValueHoveredTextStyle
			                        : ValueDefaultTextStyle;
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
