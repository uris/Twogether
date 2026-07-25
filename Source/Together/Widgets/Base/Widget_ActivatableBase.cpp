// Uris - All Rights Reserved


#include "Widgets/Base/Widget_ActivatableBase.h"

#include "TogetherUIPlayerController.h"
#include "UIFunctionLibrary.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "Widgets/Components/UICommonTipText.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UI/UISubsystem.h"

// core settings
void UWidget_ActivatableBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// store subsystem
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		UI = GameInstance->GetSubsystem<UUISubsystem>();
	}
}

// setup: registration of event delegates, etc.
void UWidget_ActivatableBase::NativeConstruct()
{
	Super::NativeConstruct();

	// register ui subsystem delegates
	if (UI)
	{
		UI->OnButtonDescriptionUpdated.AddUniqueDynamic(this, &UWidget_ActivatableBase::HandleButtonDescriptionUpdate);
		UI->OnHelpTextUpdated.AddUniqueDynamic(this, &UWidget_ActivatableBase::HandleHelpTextUpdate);
	}
}

// cleanup: de-register delegates, etc.
void UWidget_ActivatableBase::NativeDestruct()
{

	// release ui subsystem delegates
	if (UI)
	{
		UI->OnButtonDescriptionUpdated.RemoveDynamic(this, &UWidget_ActivatableBase::HandleButtonDescriptionUpdate);
		UI->OnHelpTextUpdated.RemoveDynamic(this, &UWidget_ActivatableBase::HandleHelpTextUpdate);
	}

	// call super
	Super::NativeDestruct();
}

void UWidget_ActivatableBase::NativeOnActivated()
{
	PendingExitType = EWidgetExitType::None;

	Super::NativeOnActivated();
}

void UWidget_ActivatableBase::NativeOnDeactivated()
{
	PendingExitType = EWidgetExitType::None;

	Super::NativeOnDeactivated();
}

// handle button rollover descriptions
void UWidget_ActivatableBase::HandleButtonDescriptionUpdate(UUICommonButtonBase* Button, const FText& Description)
{

	// active help text takes priority
	if (HelpTextBox && !HelpTextBox->GetTipText().IsEmpty())
	{
		return;
	}

	// protect for emitter button
	if (!Button)
	{
		return;
	}

	// check the emitter button is in the current widget
	const UWidget_ActivatableBase* OwningScreen = Button->GetTypedOuter<UWidget_ActivatableBase>();
	if (OwningScreen != this)
	{
		return;
	}

	if (DescriptionTextBox)
	{
		DescriptionTextBox->SetTipText(Description);
	}
}

// handle help text to be displayed
void UWidget_ActivatableBase::HandleHelpTextUpdate(UObject* Object, const FText& HelpText) const
{

	// remove button description if incoming help text competes for display
	if (HelpTextBox && !HelpText.IsEmpty() && DescriptionTextBox)
	{
		DescriptionTextBox->SetTipText(FText::GetEmpty());
	}

	// protect for emitter button
	if (!Object)
	{
		return;
	}

	// check the emitter button is in the current widget
	const UWidget_ActivatableBase* OwningScreen = Object->GetTypedOuter<UWidget_ActivatableBase>();
	if (OwningScreen != this)
	{
		return;
	}

	if (HelpTextBox)
	{
		HelpTextBox->SetTipText(HelpText);
	}
}

ATogetherUIPlayerController* UWidget_ActivatableBase::GetOwningUiPc()
{
	if (!CachedOwningUiPc.IsValid())
	{
		CachedOwningUiPc = GetOwningPlayer<ATogetherUIPlayerController>();
	}

	return CachedOwningUiPc.IsValid() ? CachedOwningUiPc.Get() : nullptr;
}

bool UWidget_ActivatableBase::RequestExit(const EWidgetExitType Type)
{
	// if action already pending or cannot be performed
	if (Type == EWidgetExitType::None || IsExitPending() || !CanRequestExit(Type))
	{
		return false;
	}

	// play optional SFX
	if (bActionsPlayClickSFX && ClickSFX.SFXTagName.IsValid())
	{
		UUIFunctionLibrary::PlaySoundFX(this, ClickSFX.SFXTagName, ClickSFX.DesiredVolume);
	}

	// set pending state
	PendingExitType = Type;

	// call prepare for exit
	PrepareForExit(Type);

	// if not transition, complete the exit
	if (!BeginExitTransition(Type))
	{
		CompleteExit();
	}

	return true;
}

void UWidget_ActivatableBase::CompleteExit()
{
	// if no exit is pending no need to call pre exit actions
	if (!IsExitPending())
	{
		return;
	}

	// reset pending state
	const EWidgetExitType Type = PendingExitType;
	PendingExitType = EWidgetExitType::None;

	// execute exit
	ExecuteExitAction(Type);
}

// overridable

bool UWidget_ActivatableBase::CanRequestExit(EWidgetExitType Type) const
{
	// defaults to true
	return true;
}

void UWidget_ActivatableBase::PrepareForExit(EWidgetExitType Type)
{
	// default no prep needed
}

bool UWidget_ActivatableBase::BeginExitTransition(EWidgetExitType Type)
{
	// no async transition exists
	return false;
}

void UWidget_ActivatableBase::ExecuteExitAction(EWidgetExitType Type)
{
	DeactivateWidget();
}

bool UWidget_ActivatableBase::IsExitPending() const
{
	return PendingExitType != EWidgetExitType::None;
}