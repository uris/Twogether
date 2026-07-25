// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "UI/UISoundFXs.h"
#include "Widget_ActivatableBase.generated.h"

struct FGameplayTag;
class UUISubsystem;
class UUICommonTipText;
class ATogetherUIPlayerController;
class UUICommonButtonBase;

UENUM(BlueprintType)
enum class EWidgetExitType : uint8
{
	Back UMETA(DisplayName = "Back"),
	Confirm UMETA(DisplayName = "Confirm"),
	Cancel UMETA(DisplayName = "Cancel"),
	Close UMETA(DisplayName = "Close"),
	None UMETA(DisplayName = "None"),
};

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class TOGETHER_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	// Bind widget element for help text display event
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UUICommonTipText> HelpTextBox;

	// Bind widget element for button description display event
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	TObjectPtr<UUICommonTipText> DescriptionTextBox;

	// Define SFX for click actions
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="Action Buttons|SoundFX")
	FUI_SFX_Setting ClickSFX = FUI_SFX_Setting();

	// Set true if default actions like back should immediately play SFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons|SoundFX")
	bool bActionsPlayClickSFX = true;

	/** delegates need to be a UFUNCTION to run on a BP implementation */

	// Handle updates to button description text
	UFUNCTION()
	void HandleButtonDescriptionUpdate(UUICommonButtonBase* Button, const FText& Description);

	// Handle updates to help text
	UFUNCTION()
	void HandleHelpTextUpdate(UObject* Object, const FText& HelpText) const;

	// helper: to get owning player controller
	UFUNCTION(BlueprintPure, Category="UI Button")
	ATogetherUIPlayerController* GetOwningUiPc();

	// helper: easy access to UI Subsystem
	UPROPERTY()
	UUISubsystem* UI;

protected:
	// set up config
	virtual void NativeOnInitialized() override;

	// override for setup of transition end delegate
	virtual void NativeConstruct() override;

	// override for releasing transition delegate
	virtual void NativeDestruct() override;

	// clear timers, setup state
	virtual void NativeOnActivated() override;

	// clear timers, setup state
	virtual void NativeOnDeactivated() override;

	// called by children to trigger exit lifecycle actions
	bool RequestExit(EWidgetExitType Type);
	void CompleteExit();

	// overrides for handling exit
	virtual bool CanRequestExit(EWidgetExitType) const;
	virtual void PrepareForExit(EWidgetExitType);
	virtual bool BeginExitTransition(EWidgetExitType);
	virtual void ExecuteExitAction(EWidgetExitType Type);

	// Getter for exit pending state
	bool IsExitPending() const;

private:
	// track pending
	// bool bExitPending = false;

	// exit type state
	EWidgetExitType PendingExitType = EWidgetExitType::None;

	// helper: pc
	TWeakObjectPtr<ATogetherUIPlayerController> CachedOwningUiPc;
};
