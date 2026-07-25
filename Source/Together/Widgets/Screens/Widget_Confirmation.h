// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SharedTypes/SharedTypes.h"
#include "Widgets/Base/Widget_ContentViewBase.h"
#include "Widget_Confirmation.generated.h"

class UImage;
class UDynamicEntryBox;
class UCommonTextBlock;
class UUICommonButtonBase;

USTRUCT(BlueprintType)
struct FConfirmationScreenButton
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfirmationButtonType ConfirmationButtonType = EConfirmationButtonType::Unknown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonText;
};

USTRUCT(BlueprintType)
struct FConfirmationButtonLabels
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OkLabel = FText::FromString("Ok");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText YesLabel = FText::FromString("Yes");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NoLabel = FText::FromString("No");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CancelLabel = FText::FromString("Cancel");
};


/**
 * Creates an object with the elements of a conformation screen widget - title, message, and dynamic actions
 */
UCLASS()
class TOGETHER_API UConfirmationScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	// create info screen object
	static UConfirmationScreenInfoObject* CreateConfirmationInfoObject(const FText& InTitle,
	                                                                   const FText& InMessage,
	                                                                   const FConfirmationButtonLabels& InButtonLabels,
	                                                                   const EConfirmationScreenType& ScreenType,
	                                                                   const FGameplayTag& InIconTag = FGameplayTag(),
	                                                                   const FGameplayTag& InSoundFX = FGameplayTag());
	UPROPERTY(Transient)
	FText ScreenTitle;

	UPROPERTY(Transient)
	FText ScreenMessage;

	UPROPERTY(Transient)
	TArray<FConfirmationScreenButton> ScreenButtons;

	UPROPERTY(Transient)
	FConfirmationButtonLabels ConfirmationLabels = FConfirmationButtonLabels();

	UPROPERTY(Transient)
	FGameplayTag IconTag;

	UPROPERTY(Transient)
	FGameplayTag SoundFXTag;

private:
	// helper: return the base info object with title and message set
	static UConfirmationScreenInfoObject* CreateInfoObject(const FText& InTitle, const FText& InMessage);

	// helper: create then add buttons to the info object
	static void AddButton(const EConfirmationButtonLabel& Label,
	                      const EConfirmationButtonType& Type,
	                      const FConfirmationButtonLabels& Labels,
	                      UConfirmationScreenInfoObject* InfoObject);
};

/**
 * Gets called from outside the class to "re" construct the confirmation screen with the needed elements
 * from the passed in info object
 */
UCLASS()
class TOGETHER_API UWidget_Confirmation : public UWidget_ContentViewBase
{
	GENERATED_BODY()

public:
	// takes in an info object with the screen elements and a lambda of the clicked button
	void InitConfirmScreen(UConfirmationScreenInfoObject* InInfoObject,
	                       const TFunction<void(EConfirmationButtonType)>& ClickedButtonCallback);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ErrorSoundFX;

protected:
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	virtual bool NativeOnHandleBackAction() override;
	virtual void ExecuteExitAction(EWidgetExitType Type) override;

	/**
	 * Optional presentation class for dynamically created confirmation buttons.
	 * Configure shared visuals and behavior on this button subclass. When unset,
	 * the DynamicEntryBox's Entry Widget Class is used.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Confirmation|Buttons")
	TSubclassOf<UUICommonButtonBase> ConfirmationButtonClass;

private:
	// button click handler
	void HandleButtonClick(const EConfirmationButtonType& ButtonType);

	// helper: reset existing buttons of the current conformation screen state
	void ResetAndClearCurrentButtons() const;

	// helper: add each new button
	void AddNewButton(const FConfirmationScreenButton& AvailableButton);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* TitleText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* MessageText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UDynamicEntryBox* ConfirmationButtons;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	UImage* IconImage;

	TFunction<void(EConfirmationButtonType)> ConfirmationResultCallback;

	TWeakObjectPtr<UWidget> PreferredFocusButton;
	EConfirmationButtonType BackButtonType = EConfirmationButtonType::Unknown;

	void ApplyOptionalIcon(const FGameplayTag& IconTag) const;

	void EmitErrorSound() const;

};
