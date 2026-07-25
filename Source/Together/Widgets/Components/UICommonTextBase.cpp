// Uris - All Rights Reserved


#include "Widgets/Components/UICommonTextBase.h"

#include "Subsystems/UI/UISubsystem.h"

void UUICommonTextBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// perform setup based on the text type
	switch (TextType)
	{
		case ETextType::ButtonDescription:
			SetupButtonDescriptionText();
			break;
		case ETextType::HelpText:
			SetupHelpText();
			break;
		default:
			return;
	}

}

void UUICommonTextBase::OnTextChanged()
{
	Super::OnTextChanged();

	// broadcast the text change
	OnTextDidChanged.Broadcast(GetText(), GetText().IsEmpty());
}

// button description text types will automatically update text based on the button hover event
void UUICommonTextBase::SetupButtonDescriptionText()
{
	if (UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(this))
	{
		// the description text is empty by default
		SetText(FText::GetEmpty());

		// bind ui system delegate for hover notifications
		UISubsystem->OnButtonDescriptionUpdated.AddUniqueDynamic(
			this,
			&ThisClass::HandleButtonDescriptionUpdated
			);
	}
}

void UUICommonTextBase::SetupHelpText()
{
	if (UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(this))
	{
		// the help text is empty by default
		SetText(FText::GetEmpty());

		// bind ui system delegate for hover notifications
		UISubsystem->OnHelpTextUpdated.AddUniqueDynamic(
			this,
			&ThisClass::HandleHelpTextUpdate
			);
	}
}

void UUICommonTextBase::HandleButtonDescriptionUpdated(UUICommonButtonBase* Button, const FText& Description)
{
	SetText(Description);
}

void UUICommonTextBase::HandleHelpTextUpdate(UObject* Object, const FText& HelpText)
{
	SetText(HelpText);
}
