// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SharedTypes/MotionValue.h"
#include "UICommonProgressText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUICommonProgressTextTransitionEvent, bool, bIsWorking);

/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonProgressText : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetWorking(FString Text = TEXT(""));

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetIdle(FString Text = TEXT(""));

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetProgress(bool bWorking);

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetCollapsed(bool bCollapse);

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void Reset();

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetDefaultText(FString Text);

	UFUNCTION(BlueprintCallable, Category="Progress Text|State")
	void SetProgressText(FString Text);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FUICommonProgressTextTransitionEvent OnTransitionStart;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FUICommonProgressTextTransitionEvent OnTransitionEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|State")
	bool bIsWorking = false;
	bool bIsCollapsed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Animation")
	FTransition Transition = FTransition(1.0f, 0.0f, 0, true, ECustomTransitionType::Linear);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Animation", meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float StartingOpacity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Animation", meta=(ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float EndingOpacity = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Content")
	FString DefaultText = TEXT("Default Text");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Content")
	FString WorkingText = TEXT("Working Text");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Style")
	TSubclassOf<UCommonTextStyle> DefaultTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom|Style")
	TSubclassOf<UCommonTextStyle> WorkingTextStyle;

protected:
	UPROPERTY(meta = (BindWidget));
	UCommonTextBlock* TextBlock;

	virtual void NativePreConstruct() override;

	virtual void SynchronizeProperties() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// helper: apply text style based on state
	void ApplyTextStyle() const;

	// helper: apply text content
	void ApplyTextValue() const;

	// helper: start/stop the working pulse
	void StartWorkingTransition();
	void StopWorkingTransition();

private:
	// manage current text displayed
	FString CurrentText = DefaultText;

	UPROPERTY()
	FMotionValue OpacityMotion;

	// runtime construction values restored by Reset
	bool bInitialValuesCaptured = false;
	bool bInitialWorking = false;
	bool bInitialCollapsed = false;
	FTransition InitialTransition;
	float InitialStartingOpacity = 1.0f;
	float InitialEndingOpacity = 0.0f;
	FString InitialDefaultText;
	FString InitialWorkingText;
	TSubclassOf<UCommonTextStyle> InitialDefaultTextStyle;
	TSubclassOf<UCommonTextStyle> InitialWorkingTextStyle;
};
