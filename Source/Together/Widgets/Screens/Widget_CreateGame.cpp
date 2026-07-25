// Uris - All Rights Reserved


#include "Widget_CreateGame.h"

#include "TimerManager.h"
#include "Components/Spacer.h"
#include "Widgets/Components/UICommonButtonBase.h"
#include "Widgets/Components/UICommonContentBoxBase.h"
#include "Widgets/Components/UICommonInputFieldBase.h"
#include "Widgets/Components/UICommonProgressText.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Utility/AutoGen.h"
#include "Utility/Debug.h"

void UWidget_CreateGame::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	MP = GI ? GI->GetSubsystem<UMultiplayerSubsystem>() : nullptr;
	if (MP)
	{
		MP->OnServerCreated.AddUniqueDynamic(this, &UWidget_CreateGame::HandleOnServerCreated);
		MP->OnCreateServerFail.AddUniqueDynamic(this, &UWidget_CreateGame::HandleOnCreateServerFail);
		MP->OnSessionActiveChange.AddUniqueDynamic(this, &UWidget_CreateGame::HandleOnSessionActiveChange);
	}

	if (Input_GameName)
	{
		Input_GameName->OnChangeInputFieldValidation.AddUniqueDynamic(
			this,
			&UWidget_CreateGame::HandleInputFieldValidationChange);
	}

	if (Btn_Create)
	{
		Btn_Create->OnButtonClick.AddUniqueDynamic(this, &UWidget_CreateGame::HandleButtonClick);
	}

	if (ContentView)
	{
		ContentView->SetCollapsed(true, true);
		ContentView->OnTransitionEnd.AddUniqueDynamic(this, &UWidget_CreateGame::HandleContentViewTransitionEnd);
	}
}

void UWidget_CreateGame::NativeDestruct()
{
	if (MP)
	{
		MP->OnServerCreated.RemoveDynamic(this, &UWidget_CreateGame::HandleOnServerCreated);
		MP->OnCreateServerFail.RemoveDynamic(this, &UWidget_CreateGame::HandleOnCreateServerFail);
		MP->OnSessionActiveChange.RemoveDynamic(this, &UWidget_CreateGame::HandleOnSessionActiveChange);
	}

	if (Input_GameName)
	{
		Input_GameName->OnChangeInputFieldValidation.RemoveDynamic(
			this,
			&UWidget_CreateGame:: HandleInputFieldValidationChange);
	}

	if (Btn_Create)
	{
		Btn_Create->OnButtonClick.RemoveDynamic(this, &UWidget_CreateGame::HandleButtonClick);
	}

	if (ContentView)
	{
		ContentView->SetCollapsed(true);
		ContentView->OnTransitionEnd.RemoveDynamic(this, &UWidget_CreateGame::HandleContentViewTransitionEnd);
	}

	Super::NativeDestruct();
}

void UWidget_CreateGame::NativeOnActivated()
{
	Super::NativeOnActivated();

	bIsCreatingGame = false;
	ResetCreateGame();
	ShowProgress(false);

	if (Input_GameName)
	{

		// seed random game name
		const FName TableName = FName("FunnyWords");
		const FText RandomName = UAutoGen::GenerateRandomWords(TableName, 3);
		Input_GameName->SetText(RandomName);

		// set focus to the input
		Input_GameName->SetFocus();
	}

	if (ContentView)
	{
		ContentView->SetCollapsed(false);
	}
}

void UWidget_CreateGame::NativeOnDeactivated()
{
	ResetCreateGame();

	Super::NativeOnDeactivated();
}

void UWidget_CreateGame::PrepareForExit(const EWidgetExitType Type)
{
	Debug::Print("BackButton");

	// Cancel any delayed create request before the shared exit transition starts.
	ResetCreateGame();

	// Restore normal chrome fading for the exit presentation.
	SetFooterShouldFade(true);
	SetHeaderShouldFade(true);

	Super::PrepareForExit(Type);
}

void UWidget_CreateGame::HandleOnServerCreated(FTogetherSession SessionState)
{
	ResetCreateGame();
	CollapseContentView(false);
}

void UWidget_CreateGame::HandleOnCreateServerFail(FString Reason, FTogetherSession SessionState) const
{
	PushErrorMessage(Reason);
}

void UWidget_CreateGame::PushErrorMessage(const FString& Reason) const
{
	Debug::Print(Reason);

	// protect for UI subsystem
	if (!UI)
	{
		Debug::Print("Unable to display error: UI subsystem is unavailable");
		return;
	}

	// build info for error message
	const FGameplayTag IconTag = FGameplayTag::RequestGameplayTag(TEXT("UI.Icons.Info"));
	const FGameplayTag SoundFXTag = FGameplayTag::RequestGameplayTag(TEXT("UI.SoundFX.State.Error"));
	FConfirmationButtonLabels ButtonLabels = FConfirmationButtonLabels();
	ButtonLabels.YesLabel = FText::FromString("Ok");
	const FText ScreenTitle = NSLOCTEXT("CreateGame", "CreateGameFailedTitle", "Unable to create your game");

	// get provider based message
	FText ScreenMessage;
	const EOnlineProvider OnlineProvider = UMultiplayerSubsystem::GetOnlineProvider(this->GetWorld());
	const FString ProviderName = UMultiplayerSubsystem::GetOnlineProviderName(this->GetWorld());
	switch (OnlineProvider)
	{
		case EOnlineProvider::Steam:
		{
			const FString Msg =
				FString::Printf(TEXT("Check that your PC is online and that %s is up and running."), *ProviderName);
			ScreenMessage = FText::FromString(Msg);
			break;
		}
		case EOnlineProvider::Null:
		case EOnlineProvider::Unknown:
		{
			ScreenMessage = FText::FromString("Check that your PC is online and connected to the local network.");
			break;
		}
	}

	// push
	UI->PushConfirmationScreenToStack(
		EConfirmationScreenType::Ok,
		ScreenTitle,
		ScreenMessage,
		ButtonLabels,
		[](EConfirmationButtonType) {},
		IconTag,
		SoundFXTag);
}

void UWidget_CreateGame::HandleOnSessionActiveChange(const bool bActive) const
{
	if (Input_GameName)
	{
		Input_GameName->SetIsEnabled(!bActive);
	}
}

void UWidget_CreateGame::HandleButtonClick(const FString& ButtonName, EAsyncButtonState ButtonState)
{
	if (!ButtonName.IsEmpty())
	{
		if (ButtonName.Equals("create_game", ESearchCase::CaseSensitive))
		{
			if (const UWorld* World = GetWorld())
			{
				World->GetTimerManager().SetTimer(
					TimerHandle,
					this,
					&ThisClass::CreateGameServer,
					CreateDelay,
					false);
			}
			// trigger transitions
			bIsCreatingGame = true;
			CollapseContentView(true);
		}
	}
}

void UWidget_CreateGame::HandleInputFieldValidationChange(UUICommonInputFieldBase* InputText,
                                                          const bool bDidValidate) const
{
	if (Btn_Create)
	{
		Btn_Create->SetIsEnabled(bDidValidate);
	}
}

void UWidget_CreateGame::HandleContentViewTransitionEnd(bool bCollapsed) const
{
	if (bIsCreatingGame)
	{
		ShowProgress(true);
	}
}

void UWidget_CreateGame::CreateGameServer()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
		if (MP && Input_GameName && Input_GameName->bIsValid)
		{
			MP->HostGame(Input_GameName->GetText().ToString());
		}
	}
}

void UWidget_CreateGame::CollapseContentView(const bool bCollapse)
{
	// prevent header and footer fade if creating server
	SetFooterShouldFade(!bIsCreatingGame);
	SetHeaderShouldFade(!bIsCreatingGame);

	if (ContentView)
	{
		ContentView->SetCollapsed(bCollapse);
	}
}

void UWidget_CreateGame::ShowProgress(const bool bProgressState) const
{
	// hide/show progress text
	if (Btn_Create && bProgressState)
	{
		if (Spacer)
		{
			Spacer->SetVisibility(ESlateVisibility::Collapsed);
		}
		Btn_Create->SetWorking();
	}
	else if (Btn_Create)
	{
		if (Spacer)
		{
			Spacer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		Btn_Create->SetStatic();
	}
}

void UWidget_CreateGame::ResetCreateGame()
{
	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
}
