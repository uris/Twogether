// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/EditableTextBox.h"
#include "SharedTypes/SharedTypes.h"

#include "UICommonInputFieldBase.generated.h"

class FWorldInterval;
// delegate firing when input validation state changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeInputFieldValidation,
                                             UUICommonInputFieldBase*,
                                             InputText,
                                             bool,
                                             bDidValidate);

/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonInputFieldBase : public UEditableTextBox
{
	GENERATED_BODY()

public:
	/** helper: sets a timeout callback to set focus on the input */
	UFUNCTION(BlueprintCallable, Category="Custom Behavior|Interaction")
	void StartAutoFocus();

	/** helper: clears timeout callback setting focus on the input */
	UFUNCTION(BlueprintCallable, Category="Custom Behavior|Interaction")
	void CancelAutoFocus();

	/** autofocus the text input on load */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Interaction")
	bool bFocusOnLoad = true;

	/** autofocus delay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Interaction")
	float FocusDelay = 2.0f;

	/**
	 * removed the hint text to show an empty text field if the is no user entered value
	 * and the text field is currently focused
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Interaction")
	bool bClearHintTextOnFocused = false;

	/** set validation performed on the value of the input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Validation")
	EInputValidationType Validation = EInputValidationType::None;

	/** set min size for validation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Validation")
	int32 MinLength = 0;

	/** set max size for validation purposes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Validation")
	int32 MaxLength = 255;

	/** current validation state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Behavior|Validation")
	bool bIsValid = true;

	/** set to true when a user enters a character into the input box for te first time */
	UPROPERTY(EditAnywhere, Category="Custom Behavior|Validation")
	bool bUserInitialized = false;

	/** color of hint text */
	UPROPERTY(EditAnywhere, Category="Custom Behavior|Text Color")
	FLinearColor HintTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.3f);

	/** regular color entered text */
	UPROPERTY(EditAnywhere, Category="Custom Behavior|Text Color")
	FLinearColor ActiveTextColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	/** tip text: displayed hint text for fields requiring on screen help/guidance */
	UPROPERTY(EditAnywhere, Category="Custom Behavior|Help Text")
	FText HelpTip;

	// notifier of validation state updates via delegate
	UPROPERTY(BlueprintAssignable, Category="UI|Events")
	FOnChangeInputFieldValidation OnChangeInputFieldValidation;

protected:
	/** constructor */
	UUICommonInputFieldBase();

	// User interaction triggered events

	/** perform custom validation on change */
	virtual void HandleOnTextChanged(const FText& ChangedText) override;

	/** broadcast on submit */
	virtual void HandleOnTextCommitted(const FText& CommitedText, ETextCommit::Type CommitMethod) override;

	// Widget core lifecycle

	/** apply the correct initial color hooking into slate object properties */
	virtual void SynchronizeProperties() override;

	virtual void OnWidgetRebuilt() override;

	/** clear autofocus on removal */
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	/** helper: update input text color for predictable hint/regular text */
	void UpdateTextColor(const FText& CurrentText);

	/** helper: override Slate's default 35% hint opacity with HintTextColor's alpha */
	void UpdateHintTextOpacity() const;

	/** perform validation */
	bool TextChangeValid(const FText& ChangedText) const;

	/** handler of the focus change delegate event registered on sync */
	void HandleFocusChanging(
		const FFocusEvent& FocusEvent,
		const FWeakWidgetPath& PreviousFocusPath,
		const TSharedPtr<SWidget>& PreviousFocusedWidget,
		const FWidgetPath& NewFocusPath,
		const TSharedPtr<SWidget>& NewFocusedWidget);
	void BroadcastHintTextChange();

	/** delegate for handle focus change */
	FDelegateHandle FocusChangingHandle;

	/* track keyboard focus */
	bool bHadKeyboardFocus = false;

	/* track original hint text */
	FText CachedHintText = FText::GetEmpty();

	/** timer for autofocus */
	TSharedPtr<FWorldInterval> FocusTimeOut = nullptr;

};


/*
Widget Lifecycle
### In game
A typical runtime sequence is:

CreateWidget()
  ↓
Initialize / NativeOnInitialized
  ↓
AddToViewport() or attach to another visible widget
  ↓
RebuildWidget()
  ↓
OnWidgetRebuilt()
  ↓
SynchronizeProperties()
  ↓
NativePreConstruct / PreConstruct
  ↓
NativeConstruct / Construct
  ↓
Slate layout pass
  ↓
Slate paint pass ← actually rendered on screen


### In the UMG editor

When the Widget Blueprint Designer opens or refreshes:

Designer requests preview
  ↓
RebuildWidget()
  ↓
OnWidgetRebuilt()
  ↓
SynchronizeProperties()
  ↓
NativePreConstruct / PreConstruct
     IsDesignTime() == true
  ↓
Designer layout and paint

OnWidgetRebuilt  → Slate object is available
PreConstruct     → configure preview/runtime appearance
Construct        → runtime hierarchy is active
Slate paint      → pixels are actually drawn

- C++ constructor — best for genuine defaults that do not require the Slate widget or runtime context.
- SynchronizeProperties() — best for pushing UPROPERTY values into your underlying Slate widget, including editor preview.
- NativePreConstruct() — useful for presentation that must differ between editor preview and runtime.
- NativeOnInitialized() — best for one-time runtime delegate binding and setup.
- NativeConstruct() — use for runtime setup that should occur each time the widget enters an active hierarchy.
 */
