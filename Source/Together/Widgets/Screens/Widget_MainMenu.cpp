// Uris - All Rights Reserved

#include "Widgets/Screens/Widget_MainMenu.h"

#include "OnlineSubsystemUtils.h"
#include "Together.h"
#include "TogetherUIGameplayTags.h"
#include "UIFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "Widgets/Components/UICommonBoundActionBarBase.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

void UWidget_MainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UI = GetGameInstance()->GetSubsystem<UUISubsystem>();
	if (UI)
	{
		ModalStack = UI->FindWidgetStackByTag(UIGameplayTags::UI_WidgetStack_Modal);
		if (ModalStack)
		{
			ModalStack->OnDisplayedWidgetChanged().AddUObject(
				this,
				&ThisClass::HandleModalStackChanged);
		}
	}

	if (Btn_Create)
	{
		Btn_Create->OnClicked().AddUObject(this, &ThisClass::HandleCreateClicked);
	}
	if (Btn_Credits)
	{
		Btn_Credits->OnClicked().AddUObject(this, &ThisClass::HandleCreditsClicked);
	}
	if (Btn_Join)
	{
		Btn_Join->OnClicked().AddUObject(this, &ThisClass::HandleJoinClicked);
	}
	if (Btn_Options)
	{
		Btn_Options->OnClicked().AddUObject(this, &ThisClass::HandleOptionsClicked);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked().AddUObject(this, &ThisClass::HandleQuitClicked);
	}
}

void UWidget_MainMenu::NativeDestruct()
{

	// remove delegate bind and reset stack pointer
	if (ModalStack)
	{
		ModalStack->OnDisplayedWidgetChanged().RemoveAll(this);
		ModalStack = nullptr;
	}

	// release button binds
	if (Btn_Create)
	{
		Btn_Create->OnClicked().RemoveAll(this);
	}
	if (Btn_Credits)
	{
		Btn_Credits->OnClicked().RemoveAll(this);
	}
	if (Btn_Join)
	{
		Btn_Join->OnClicked().RemoveAll(this);
	}
	if (Btn_Options)
	{
		Btn_Options->OnClicked().RemoveAll(this);
	}
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UWidget_MainMenu::NativeOnActivated()
{
	Super::NativeOnActivated();
	HandleMainMenuResumed();
}

void UWidget_MainMenu::HandleModalStackChanged(UCommonActivatableWidget* DisplayedWidget)
{
	// if there's an active widget on the modal stack, main menu has an overlay
	if (DisplayedWidget)
	{
		if (BoundActionBar)
		{
			BoundActionBar->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	// otherwise, if this is activated, the main menu has resumed
	if (IsActivated())
	{
		HandleMainMenuResumed();
	}
}

void UWidget_MainMenu::HandleMainMenuResumed() const
{
	// reset the bound action bar to visible
	if (BoundActionBar)
	{
		BoundActionBar->SetVisibility(ESlateVisibility::Visible);
	}

	if (UWidget* FocusTarget = GetDesiredFocusTarget())
	{
		FocusTarget->SetFocus();
	}
}

bool UWidget_MainMenu::NativeOnHandleBackAction()
{
	if (bActionsPlayClickSFX && ClickSFX.SFXTagName.IsValid())
	{
		UUIFunctionLibrary::PlaySoundFX(this, ClickSFX.SFXTagName, ClickSFX.DesiredVolume);
	}

	return Super::NativeOnHandleBackAction();
}

UWidget* UWidget_MainMenu::NativeGetDesiredFocusTarget() const
{
	return Btn_Create ? Btn_Create.Get() : Super::NativeGetDesiredFocusTarget();
}

void UWidget_MainMenu::HandleCreateClicked() const
{
	PushActivatableWidgetScreen(UIGameplayTags::UI_Widget_HostNewGame, UIGameplayTags::UI_WidgetStack_Modal);
}

void UWidget_MainMenu::HandleCreditsClicked()
{
	OnCreditsRequested();
}

void UWidget_MainMenu::HandleJoinClicked() const
{
	PushActivatableWidgetScreen(UIGameplayTags::UI_Widget_JoinGame, UIGameplayTags::UI_WidgetStack_Modal);
}

void UWidget_MainMenu::HandleOptionsClicked() const
{
	PushActivatableWidgetScreen(UIGameplayTags::UI_Widget_Options, UIGameplayTags::UI_WidgetStack_FrontEnd);
}

void UWidget_MainMenu::HandleQuitClicked()
{
	if (!UI)
	{
		return;
	}

	FConfirmationButtonLabels Labels;
	Labels.OkLabel = NSLOCTEXT("MainMenu", "QuitConfirm", "Quit");
	Labels.CancelLabel = NSLOCTEXT("MainMenu", "QuitCancel", "Cancel");

	UI->PushConfirmationScreenToStack(
		EConfirmationScreenType::OkCancel,
		QuitTitle,
		QuitMessage,
		Labels,
		[this](const EConfirmationButtonType Result)
		{
			HandleQuitResult(Result);
		});
}

void UWidget_MainMenu::HandleQuitResult(const EConfirmationButtonType Result)
{
	if (Result == EConfirmationButtonType::Confirm)
	{
		UUISubsystem::ResetInputAndQuitGame(this);
	}
}

void UWidget_MainMenu::PushActivatableWidgetScreen(const FGameplayTag& WidgetTag,
                                                   const FGameplayTag& WidgetStackTag) const
{
	if (!UI)
	{
		return;
	}

	const TSoftClassPtr<UWidget_ActivatableBase> ScreenClass =
		UUIFunctionLibrary::GetFrontEndSoftWidgetClassByTag(WidgetTag);
	if (ScreenClass.IsNull())
	{
		return;
	}

	if (BoundActionBar)
	{
		BoundActionBar->SetVisibility(ESlateVisibility::Collapsed);
	}

	const TWeakObjectPtr<APlayerController> OwningPlayer = GetOwningPlayer();
	UI->PushSoftWidgetToStack(
		WidgetStackTag,
		ScreenClass,
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
