// Uris - All Rights Reserved


#include "Widgets/Screens/Widget_Confirmation.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Components/Image.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "Widgets/Components/UICommonContentBoxBase.h"
#include "SharedTypes/SharedTypes.h"
#include "Subsystems/UI/UISubsystem.h"
#include "UIFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UIIconSet.h"
#include "UI/UISoundFXs.h"
#include "Utility/Debug.h"

// ** UConfirmationScreenInfoObject **/

UConfirmationScreenInfoObject* UConfirmationScreenInfoObject::CreateConfirmationInfoObject(const FText& InTitle,
	const FText& InMessage,
	const FConfirmationButtonLabels& InButtonLabels,
	const EConfirmationScreenType& ScreenType,
	const FGameplayTag& InIconTag,
	const FGameplayTag& InSoundFX)
{

	// create the base info object
	UConfirmationScreenInfoObject* InfoObject = CreateInfoObject(InTitle, InMessage);
	InfoObject->IconTag = InIconTag;
	InfoObject->SoundFXTag = InSoundFX;

	// based on then confirmation screen type add the necessary buttons to the info object
	switch (ScreenType)
	{

		case EConfirmationScreenType::Ok:
			AddButton(EConfirmationButtonLabel::Yes, EConfirmationButtonType::Close, InButtonLabels, InfoObject);
			break;
		case EConfirmationScreenType::YesNo:
			AddButton(EConfirmationButtonLabel::Yes, EConfirmationButtonType::Confirm, InButtonLabels, InfoObject);
			AddButton(EConfirmationButtonLabel::No, EConfirmationButtonType::Cancel, InButtonLabels, InfoObject);
			break;
		case EConfirmationScreenType::OkCancel:
			AddButton(EConfirmationButtonLabel::Ok, EConfirmationButtonType::Confirm, InButtonLabels, InfoObject);
			AddButton(EConfirmationButtonLabel::Cancel, EConfirmationButtonType::Cancel, InButtonLabels, InfoObject);
			break;
		case EConfirmationScreenType::Unknown:
			break;
	}

	return InfoObject;
}

// Create the base info object for a confirmation dialog
UConfirmationScreenInfoObject* UConfirmationScreenInfoObject::CreateInfoObject(const FText& InTitle,
                                                                               const FText& InMessage)
{
	UConfirmationScreenInfoObject* InfoObject = NewObject<UConfirmationScreenInfoObject>();
	InfoObject->ScreenTitle = InTitle;
	InfoObject->ScreenMessage = InMessage;
	return InfoObject;
}

// Create and add buttons to the info objects screen button array
void UConfirmationScreenInfoObject::AddButton(const EConfirmationButtonLabel& Label,
                                              const EConfirmationButtonType& Type,
                                              const FConfirmationButtonLabels& Labels,
                                              UConfirmationScreenInfoObject* InfoObject)
{
	FConfirmationScreenButton Button;
	Button.ConfirmationButtonType = Type;

	switch (Label)
	{
		case EConfirmationButtonLabel::Yes:
			Button.ButtonText = Labels.YesLabel;
			break;
		case EConfirmationButtonLabel::Ok:
			Button.ButtonText = Labels.OkLabel;;
			break;
		case EConfirmationButtonLabel::No:
			Button.ButtonText = Labels.NoLabel;;
			break;
		case EConfirmationButtonLabel::Cancel:
			Button.ButtonText = Labels.CancelLabel;
			break;
		default:
			break;
	}

	InfoObject->ScreenButtons.Add(Button);
}

// ** UWidget_Confirmation **/

void UWidget_Confirmation::InitConfirmScreen(UConfirmationScreenInfoObject* InInfoObject,
                                             const TFunction<void(EConfirmationButtonType)>& ClickedButtonCallback)
{
	// safeguard using check macro for core data elements of info screen
	check(InInfoObject && TitleText && MessageText && ConfirmationButtons);

	// set title and message text, icon and sound
	TitleText->SetText(InInfoObject->ScreenTitle);
	MessageText->SetText(InInfoObject->ScreenMessage);
	ErrorSoundFX = InInfoObject->SoundFXTag;
	ConfirmationResultCallback = ClickedButtonCallback;
	ApplyOptionalIcon(InInfoObject->IconTag);

	// set title invisible if text is empty
	const ESlateVisibility TitleVisibility = InInfoObject->ScreenTitle.IsEmpty()
		                                         ? ESlateVisibility::Hidden
		                                         : ESlateVisibility::Visible;
	TitleText->SetVisibility(TitleVisibility);

	// rest existing buttons - we only use one modal instance so need to reset and clear button states
	PreferredFocusButton.Reset();
	BackButtonType = EConfirmationButtonType::Unknown;
	ResetAndClearCurrentButtons();

	// stop if there are no buttons to add (should not happen)
	check(!InInfoObject->ScreenButtons.IsEmpty());

	// add each new button to the confirmation screen instance
	for (const FConfirmationScreenButton& AvailableButton : InInfoObject->ScreenButtons)
	{
		// Prefer Cancel as the dialog's Back result. A Close button is the
		// fallback for acknowledgement-only dialogs.
		if (AvailableButton.ConfirmationButtonType == EConfirmationButtonType::Cancel
		    || (BackButtonType == EConfirmationButtonType::Unknown
		        && AvailableButton.ConfirmationButtonType == EConfirmationButtonType::Close))
		{
			BackButtonType = AvailableButton.ConfirmationButtonType;
		}

		AddNewButton(AvailableButton);
	}

	// set focus on the last button from the added buttons
	if (ConfirmationButtons->GetNumEntries() > 0)
	{

		// get the last button
		UUserWidget* LastButton = ConfirmationButtons->GetAllEntries().Last();

		// when setting focus on last button automatically, set auto activated flag to prevent click sound from firing
		if (UUICommonButtonBase* UIButton = Cast<UUICommonButtonBase>(LastButton))
		{
			if (const UUISubsystem* UISubSystem = UUISubsystem::GetUISubsystem(UIButton))
			{
				const bool bIsGamepad = UISubSystem->IsGamepadInputMode(UIButton);
				UIButton->SetAutoActivated(bIsGamepad);
			}
		}

		// InitConfirmScreen runs before the widget is pushed/activated. Store the
		// target and let CommonUI request it at the correct point in activation.
		PreferredFocusButton = LastButton;
	}

}

void UWidget_Confirmation::NativeDestruct()
{
	ConfirmationResultCallback = nullptr;
	PreferredFocusButton.Reset();
	BackButtonType = EConfirmationButtonType::Unknown;

	Super::NativeDestruct();
}

UWidget* UWidget_Confirmation::NativeGetDesiredFocusTarget() const
{
	return PreferredFocusButton.IsValid()
		       ? PreferredFocusButton.Get()
		       : Super::NativeGetDesiredFocusTarget();
}

bool UWidget_Confirmation::NativeOnHandleBackAction()
{
	if (BackButtonType != EConfirmationButtonType::Unknown)
	{
		HandleButtonClick(BackButtonType);
		return true;
	}

	return Super::NativeOnHandleBackAction();
}

void UWidget_Confirmation::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (ContentView)
	{
		// A stack can reuse this widget, so always establish the opening state.
		ContentView->SetCollapsed(false);
	}

	// play error sound
	EmitErrorSound();
}

void UWidget_Confirmation::ExecuteExitAction(const EWidgetExitType Type)
{
	EConfirmationButtonType Result = EConfirmationButtonType::Unknown;
	switch (Type)
	{
		case EWidgetExitType::Confirm:
			Result = EConfirmationButtonType::Confirm;
			break;
		case EWidgetExitType::Cancel:
			Result = EConfirmationButtonType::Cancel;
			break;
		case EWidgetExitType::Close:
			Result = EConfirmationButtonType::Close;
			break;
		default:
			break;
	}

	TFunction<void(EConfirmationButtonType)> ResultCallback = MoveTemp(ConfirmationResultCallback);
	if (Result != EConfirmationButtonType::Unknown && ResultCallback)
	{
		ResultCallback(Result);
	}

	Super::ExecuteExitAction(Type);
}

void UWidget_Confirmation::ApplyOptionalIcon(const FGameplayTag& IconTag) const
{
	if (!IconImage)
	{
		return;
	}

	FUIIconDefinition IconDefinition;
	if (!UUIFunctionLibrary::FindUIIconDefinition(IconTag, IconDefinition))
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UTexture2D* Texture = IconDefinition.Texture.LoadSynchronous();
	if (!Texture)
	{
		IconImage->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	IconImage->SetBrushFromTexture(Texture, true);
	IconImage->SetDesiredSizeOverride(IconDefinition.DesiredSize);
	IconImage->SetColorAndOpacity(IconDefinition.Tint);
	IconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

// reset / clear buttons that are in the conformation screen first
void UWidget_Confirmation::ResetAndClearCurrentButtons() const
{
	if (ConfirmationButtons->GetNumEntries() > 0)
	{
		// define reset / clear lambda function
		const TFunction<void(UUICommonButtonBase&)> ClearCallback = [](const UUICommonButtonBase& ExistingButton)
		{
			ExistingButton.OnClicked().Clear();
		};

		// call reset with the lambda function
		ConfirmationButtons->Reset<UUICommonButtonBase>(ClearCallback);
	}
}

// add each new button to the confirmation screen object
void UWidget_Confirmation::AddNewButton(const FConfirmationScreenButton& AvailableButton)
{
	// create an entry for the button to add in the dynamic entry box
	UUICommonButtonBase* AddedButton = ConfirmationButtons->CreateEntry<UUICommonButtonBase>(ConfirmationButtonClass);
	check(AddedButton);

	// Store the label as button state as well as applying it to the current text
	// widget. This keeps it intact if the button reapplies its state later.
	AddedButton->ButtonLabel = AvailableButton.ButtonText;
	AddedButton->SetStatic();

	// register the click lambda function that runs on click
	AddedButton->OnClicked().AddLambda([AvailableButton, this]()
	{
		HandleButtonClick(AvailableButton.ConfirmationButtonType);
	});
}

void UWidget_Confirmation::HandleButtonClick(const EConfirmationButtonType& ButtonType)
{
	EWidgetExitType ExitType = EWidgetExitType::None;
	switch (ButtonType)
	{
		case EConfirmationButtonType::Confirm:
			ExitType = EWidgetExitType::Confirm;
			break;
		case EConfirmationButtonType::Cancel:
			ExitType = EWidgetExitType::Cancel;
			break;
		case EConfirmationButtonType::Close:
			ExitType = EWidgetExitType::Close;
			break;
		default:
			return;
	}

	RequestExit(ExitType);
}

void UWidget_Confirmation::EmitErrorSound() const
{

	Debug::Print(ErrorSoundFX.ToString());
	if (ErrorSoundFX.ToString().IsEmpty())
	{
		return;
	}

	FUISoundFXDefinition SoundFXDefinition;
	if (!UUIFunctionLibrary::FindSoundFXDefinition(ErrorSoundFX, SoundFXDefinition))
	{
		Debug::Print("No SoundFXDefinition");
		return;
	}

	USoundBase* Sound = SoundFXDefinition.Sound.LoadSynchronous();
	const float Volume = SoundFXDefinition.DesiredSoundLevel;
	if (!Sound)
	{
		Debug::Print("No Sound");
		return;
	}

	UGameplayStatics::PlaySound2D(this, Sound, Volume);
}