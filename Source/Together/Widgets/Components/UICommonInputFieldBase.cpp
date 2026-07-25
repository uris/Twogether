// Uris - All Rights Reserved

#include "Widgets/Components/UICommonInputFieldBase.h"

#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Widgets/Input/SEditableText.h"
#include "Utility/WorldInterval.h"

/** update slate logic to make sure hint text obeys color alphas */
namespace
{
TSharedPtr<SEditableText> FindEditableText(const TSharedRef<SWidget>& Widget)
{
	if (Widget->GetType() == FName(TEXT("SEditableText")))
	{
		return StaticCastSharedRef<SEditableText>(Widget);
	}

	FChildren* Children = Widget->GetChildren();
	for (int32 ChildIndex = 0; ChildIndex < Children->Num(); ++ChildIndex)
	{
		if (TSharedPtr<SEditableText> EditableText = FindEditableText(Children->GetChildAt(ChildIndex)))
		{
			return EditableText;
		}
	}

	return nullptr;
}
}

UUICommonInputFieldBase::UUICommonInputFieldBase()
{
	// input BG defaults
	WidgetStyle.BackgroundImageNormal.DrawAs = ESlateBrushDrawType::NoDrawType;
	WidgetStyle.BackgroundImageHovered.DrawAs = ESlateBrushDrawType::NoDrawType;
	WidgetStyle.BackgroundImageFocused.DrawAs = ESlateBrushDrawType::NoDrawType;
	WidgetStyle.BackgroundImageReadOnly.DrawAs = ESlateBrushDrawType::NoDrawType;

	// Padding
	this->WidgetStyle.Padding = 0.0f;

	// justification
	SetJustification(ETextJustify::Center);

	// Color
	this->WidgetStyle.BackgroundColor = FColor::Transparent;
	UpdateTextColor(this->GetText());

	// validation
	if (Validation == EInputValidationType::None)
	{
		bIsValid = true;
	}

	//
}

/** use slate override to register focus change call back */
void UUICommonInputFieldBase::OnWidgetRebuilt()
{
	// call parent
	Super::OnWidgetRebuilt();

	// register handler for focus event fired by slate
	if (FSlateApplication::IsInitialized() &&
	    !FocusChangingHandle.IsValid())
	{
		FocusChangingHandle =
			FSlateApplication::Get().OnFocusChanging().AddUObject(
				this,
				&UUICommonInputFieldBase::HandleFocusChanging);
	}
}


void UUICommonInputFieldBase::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// update cached hint text if we'll need to remove it on focus
	if (bClearHintTextOnFocused)
	{
		CachedHintText = GetHintText();
	}

	// update text colors
	UpdateTextColor(GetText());
	UpdateHintTextOpacity();

	// update hint text
	BroadcastHintTextChange();
}

//** override to release delegate registration and perform timer clean up */
void UUICommonInputFieldBase::ReleaseSlateResources(
	const bool bReleaseChildren)
{
	// clear timeout for autofocus
	CancelAutoFocus();

	// remove focus handler
	if (FSlateApplication::IsInitialized() &&
	    FocusChangingHandle.IsValid())
	{
		FSlateApplication::Get().OnFocusChanging().Remove(
			FocusChangingHandle);

		FocusChangingHandle.Reset();
	}

	// update focus state
	bHadKeyboardFocus = false;

	// call parent
	Super::ReleaseSlateResources(bReleaseChildren);
}

void UUICommonInputFieldBase::HandleOnTextChanged(const FText& ChangedText)
{

	CancelAutoFocus();
	Super::HandleOnTextChanged(ChangedText);
	UpdateTextColor(ChangedText);

	// Your custom behavior here.
	bUserInitialized = true;

	// perform validation as needed
	if (Validation != EInputValidationType::None)
	{
		const bool bEvaluatesValid = TextChangeValid(ChangedText);
		if (bEvaluatesValid != bIsValid)
		{
			// update valid state
			bIsValid = bEvaluatesValid;

			// broadcast validation change in state
			OnChangeInputFieldValidation.Broadcast(this, bIsValid);

			// broadcast hint text change
			BroadcastHintTextChange();
		}
	}

}

void UUICommonInputFieldBase::HandleOnTextCommitted(const FText& CommitedText,
                                                    const ETextCommit::Type CommitMethod)
{
	// if not valid, does nothing
	if (!bIsValid)
	{
		return;
	}

	CancelAutoFocus();
	Super::HandleOnTextCommitted(CommitedText, CommitMethod);
}

void UUICommonInputFieldBase::UpdateTextColor(const FText& CurrentText)
{
	SetForegroundColor(CurrentText.IsEmpty() ? HintTextColor : ActiveTextColor);
}

void UUICommonInputFieldBase::UpdateHintTextOpacity() const
{
	if (MyEditableTextBlock.IsValid())
	{
		if (const TSharedPtr<SEditableText> EditableText = FindEditableText(MyEditableTextBlock.ToSharedRef()))
		{
			EditableText->SetHintTextOpacity(HintTextColor.A);
		}
	}
}

bool UUICommonInputFieldBase::TextChangeValid(const FText& ChangedText) const
{

	bool bEvaluatesValid = true;

	// perform validation on change
	switch (Validation)
	{
		case EInputValidationType::Length:
			if (ChangedText.IsEmpty())
			{
				bEvaluatesValid = false;
				break;
			}
			else
			{
				const int32 Len = ChangedText.ToString().Len();
				if (Len < MinLength || Len > MaxLength)
				{
					bEvaluatesValid = false;
				}
				break;
			}
		case EInputValidationType::Email:
		case EInputValidationType::Phone:
		case EInputValidationType::None:
			break;
	}

	return bEvaluatesValid;
}

void UUICommonInputFieldBase::StartAutoFocus()
{
	CancelAutoFocus();

	if (!bFocusOnLoad || FocusDelay <= 0.0f)
	{
		return;
	}

	const TWeakObjectPtr<UUICommonInputFieldBase> WeakThis(this);

	FocusTimeOut = FWorldInterval::SetInterval(
		this,
		[WeakThis]()
		{
			if (WeakThis.IsValid() &&
			    WeakThis->IsVisible() &&
			    WeakThis->GetIsEnabled())
			{
				WeakThis->SetKeyboardFocus();
			}
		},
		0.1f,
		FocusDelay,
		false
		);
}

void UUICommonInputFieldBase::CancelAutoFocus()
{
	FWorldInterval::ClearInterval(FocusTimeOut);
}

/** handle gain focus and loose focus */
void UUICommonInputFieldBase::HandleFocusChanging(
	const FFocusEvent& FocusEvent,
	const FWeakWidgetPath& PreviousFocusPath,
	const TSharedPtr<SWidget>& PreviousFocusedWidget,
	const FWidgetPath& NewFocusPath,
	const TSharedPtr<SWidget>& NewFocusedWidget)
{
	if (!MyEditableTextBlock.IsValid())
	{
		return;
	}

	// The focused widget may be the internal SEditableText, so inspect
	// the complete focus path rather than comparing only the leaf.
	const bool bHasKeyboardFocus =
		NewFocusPath.ContainsWidget(MyEditableTextBlock.Get());

	// Focus gained
	if (bHasKeyboardFocus && !bHadKeyboardFocus)
	{
		if (bClearHintTextOnFocused)
		{
			SetHintText(FText::GetEmpty());
		}
	}
	// Focus lost
	else if (!bHasKeyboardFocus && bHadKeyboardFocus)
	{
		if (bClearHintTextOnFocused)
		{
			SetHintText(CachedHintText);
		}
	}

	bHadKeyboardFocus = bHasKeyboardFocus;
}

void UUICommonInputFieldBase::BroadcastHintTextChange()
{
	if (!HelpTip.IsEmpty())
	{
		if (UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(this))
		{
			FText TextToBroadcast = bIsValid ? FText::GetEmpty() : HelpTip;
			UISubsystem->OnHelpTextUpdated.Broadcast(this, TextToBroadcast);
		}

	}
}
