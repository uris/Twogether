// Uris - All Rights Reserved

#include "Widgets/Screens/Widget_PressAKey.h"

#include "Input/Reply.h"
#include "Input/UIActionBindingHandle.h"
#include "TogetherUIGameplayTags.h"
#include "UIFunctionLibrary.h"
#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Subsystems/UI/UISubsystem.h"

TOptional<FUIInputConfig> UWidget_PressAKey::GetDesiredInputConfig() const
{
	return FUIInputConfig(
		ECommonInputMode::Menu,
		EMouseCaptureMode::NoCapture,
		EMouseLockMode::DoNotLock,
		false);
}

void UWidget_PressAKey::NativeOnActivated()
{
	Super::NativeOnActivated();

	bPushPending = false;
	StopAllAnimations();
	TitleOpacityMotion.Configure(
		FMath::Clamp(TitleStartingOpacity, 0.0f, 1.0f),
		FMath::Clamp(TitleEndingOpacity, 0.0f, 1.0f),
		TitleTransition);
	PromptOpacityMotion.Configure(
		FMath::Clamp(PromptStartingOpacity, 0.0f, 1.0f),
		FMath::Clamp(PromptEndingOpacity, 0.0f, 1.0f),
		PromptTransition);

	if (TitleWrapper)
	{
		TitleOpacityMotion.Play();
		TitleWrapper->SetRenderOpacity(TitleOpacityMotion.GetValue());
	}
	else
	{
		StartPromptMotion();
	}

	if (Txt_PressKey)
	{
		Txt_PressKey->SetRenderOpacity(PromptOpacityMotion.GetValue());
	}
}

void UWidget_PressAKey::NativeOnDeactivated()
{
	StopAllAnimations();
	TitleOpacityMotion.Stop(true);
	PromptOpacityMotion.Stop(true);
	bPushPending = false;

	Super::NativeOnDeactivated();
}

void UWidget_PressAKey::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TitleOpacityMotion.IsPlaying())
	{
		if (TitleOpacityMotion.Tick(InDeltaTime) && TitleWrapper)
		{
			TitleWrapper->SetRenderOpacity(TitleOpacityMotion.GetValue());
		}

		if (TitleOpacityMotion.DidComplete())
		{
			StartPromptMotion();
			return;
		}
	}

	if (PromptOpacityMotion.IsPlaying())
	{
		if (PromptOpacityMotion.Tick(InDeltaTime) && Txt_PressKey)
		{
			Txt_PressKey->SetRenderOpacity(PromptOpacityMotion.GetValue());
		}
	}
}

UWidget* UWidget_PressAKey::NativeGetDesiredFocusTarget() const
{
	return const_cast<UWidget_PressAKey*>(this);
}

FReply UWidget_PressAKey::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	PushMainMenu();
	return FReply::Handled();
}

FReply UWidget_PressAKey::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	PushMainMenu();
	return FReply::Handled();
}

void UWidget_PressAKey::StartPromptMotion()
{
	if (Txt_PressKey && IsActivated())
	{
		PromptOpacityMotion.Play();
		Txt_PressKey->SetRenderOpacity(PromptOpacityMotion.GetValue());
	}
}

void UWidget_PressAKey::PushMainMenu()
{
	if (bPushPending || !UI)
	{
		return;
	}

	const TSoftClassPtr<UWidget_ActivatableBase> MainMenuClass =
		UUIFunctionLibrary::GetFrontEndSoftWidgetClassByTag(UIGameplayTags::UI_Widget_MainMenu);
	if (MainMenuClass.IsNull())
	{
		return;
	}

	bPushPending = true;
	if (ContinueSound.IsValid())
	{
		UUIFunctionLibrary::PlaySoundFX(this, ContinueSound);
	}

	const TWeakObjectPtr<APlayerController> OwningPlayer = GetOwningPlayer();
	UI->PushSoftWidgetToStack(
		UIGameplayTags::UI_WidgetStack_FrontEnd,
		MainMenuClass,
		[OwningPlayer](const EAsyncPushWidgetState State, UWidget_ActivatableBase* PushedWidget)
		{
			if (!PushedWidget)
			{
				return;
			}

			if (State == EAsyncPushWidgetState::OnBeforePush)
			{
				PushedWidget->SetOwningPlayer(OwningPlayer.Get());
			}
			else if (State == EAsyncPushWidgetState::OnAfterPush)
			{
				if (UWidget* FocusTarget = PushedWidget->GetDesiredFocusTarget())
				{
					FocusTarget->SetFocus();
				}
			}
		});
}
