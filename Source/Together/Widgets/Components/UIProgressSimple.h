// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SharedTypes/MotionValue.h"
#include "SharedTypes/SharedTypes.h"
#include "UIProgressSimple.generated.h"

class UBorder;
class UCanvasPanel;
class USizeBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUIProgressSimpleEvent);

/** A looping, indeterminate progress bar that wipes from left to right. */
UCLASS()
class TOGETHER_API UUIProgressSimple : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Width of both the clipped track and the moving bar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Style", meta=(ClampMin="0.0", UIMin="0.0"))
	float Width = 300.0f;

	/** Height of both the clipped track and the moving bar. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Style", meta=(ClampMin="0.0", UIMin="0.0"))
	float Height = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Style")
	FLinearColor TrackColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.15f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Style")
	FLinearColor BarColor = FLinearColor::White;

	/** Duration, delay, repetition, and easing for each left-to-right wipe. RepeatTimes 0 loops forever. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Animation")
	FTransition Transition = FTransition(1.0f, 0.25f, 0, false, ECustomTransitionType::Linear, 0.25f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Simple Progress|Animation")
	bool bAutoStart = true;

	UPROPERTY(BlueprintAssignable, Category="Simple Progress|Animation")
	FUIProgressSimpleEvent OnTransitionStart;

	UPROPERTY(BlueprintAssignable, Category="Simple Progress|Animation")
	FUIProgressSimpleEvent OnTransitionEnd;

	UFUNCTION(BlueprintCallable, Category="Simple Progress|Animation")
	void Start();

	UFUNCTION(BlueprintCallable, Category="Simple Progress|Animation")
	void Stop();

	UFUNCTION(BlueprintCallable, Category="Simple Progress|Style")
	void ApplyConfiguration();

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void SetBarPosition(float X) const;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<USizeBox> TrackSizeBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UBorder> TrackBorder;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UCanvasPanel> ProgressCanvas;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<USizeBox> BarSizeBox;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UBorder> BarBorder;

	UPROPERTY()
	FMotionValue BarMotion;
};
