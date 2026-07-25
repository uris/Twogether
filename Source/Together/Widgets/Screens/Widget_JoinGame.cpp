// Uris - All Rights Reserved


#include "Widget_JoinGame.h"

#include "Together.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Widgets/Components/UICommonContentBoxBase.h"
#include "Widgets/Components/UICommonListViewSessionsBase.h"
#include "Widgets/Components/UICommonProgressText.h"
#include "Engine/GameInstance.h"
#include "Subsystems/Multiplayer/MultiplayerSubsystem.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Utility/Debug.h"
#include "Widgets/Components/UIProgressSimple.h"

void UWidget_JoinGame::NativeConstruct()
{
	Super::NativeConstruct();

	const UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	MP = GI ? GI->GetSubsystem<UMultiplayerSubsystem>() : nullptr;
	if (MP)
	{
		MP->OnGameSessionsFound.AddUniqueDynamic(this, &UWidget_JoinGame::HandleFindGamesComplete);
		MP->OnFindSessionsFail.AddUniqueDynamic(this, &UWidget_JoinGame::HandleFindGamesFailed);
		MP->OnJoinSessionFailed.AddUniqueDynamic(this, &UWidget_JoinGame::HandleJoinSessionFailed);
		MP->OnJoinedGameSession.AddUniqueDynamic(this, &UWidget_JoinGame::HandleJoinedGameSession);
	}

	if (SessionsList)
	{
		SessionsList->OnSessionListItemClicked.AddUniqueDynamic(this, &UWidget_JoinGame::HandleGameSessionSelected);
	}

	if (SizeBox && ContentView)
	{
		SizeBox->SetMaxDesiredHeight(ContentView->Height);
	}
}

void UWidget_JoinGame::NativeDestruct()
{
	if (MP)
	{
		MP->OnGameSessionsFound.RemoveDynamic(this, &UWidget_JoinGame::HandleFindGamesComplete);
		MP->OnFindSessionsFail.RemoveDynamic(this, &UWidget_JoinGame::HandleFindGamesFailed);
		MP->OnJoinSessionFailed.RemoveDynamic(this, &UWidget_JoinGame::HandleJoinSessionFailed);
		MP->OnJoinedGameSession.RemoveDynamic(this, &UWidget_JoinGame::HandleJoinedGameSession);
	}

	if (SessionsList)
	{
		SessionsList->OnSessionListItemClicked.RemoveDynamic(this, &UWidget_JoinGame::HandleGameSessionSelected);
	}

	Super::NativeDestruct();
}

void UWidget_JoinGame::NativeOnActivated()
{
	Super::NativeOnActivated();

	// reset session state
	bIsSearching = false;
	bInitialized = false;
	bHasResults = false;
	bIsJoiningGame = false;
	bShouldJoinGame = false;
	CurrentGameSessions.Reset();

	// start search for sessions
	if (MP)
	{
		bIsSearching = true;
		MP->CancelJoinGame();
		MP->FindGameSessions();
	}

	// set view state
	FadeChrome(false);
	UpdateViewState();

	// set initialized
	bInitialized = true;
}

void UWidget_JoinGame::NativeOnDeactivated()
{
	// cancel joins
	if (MP)
	{
		MP->CancelJoinGame();
	}

	// collapse view
	FadeChrome(true);
	SetListViewState(true);

	Super::NativeOnDeactivated();
}

void UWidget_JoinGame::PrepareForExit(const EWidgetExitType Type)
{
	// Cancel any outstanding session operation before the shared exit transition.
	if (MP)
	{
		MP->CancelJoinGame();
	}

	// force headers to transition with content view
	FadeChrome(true);

	Super::PrepareForExit(Type);
}

void UWidget_JoinGame::HandleFindGamesComplete(const TArray<FTogetherSessionItem>& FoundSessions)
{
	FString Message = FString::Printf(TEXT("Num Sessions: %u"), FoundSessions.Num());
	Debug::Print(Message);

	bIsSearching = false;
	bHasResults = FoundSessions.Num() > 0;
	CurrentGameSessions = FoundSessions;

	if (SessionsList)
	{
		SessionsList->SetGameSessions(FoundSessions);
	}

	UpdateViewState();

}

void UWidget_JoinGame::HandleFindGamesFailed(FString Reason, FTogetherSession SessionState)
{
	Debug::Print("HandleFindGamesFailed");

	bIsSearching = false;
	UpdateViewState();
	PushErrorMessage(EErrorType::FindFailed, Reason);
}

void UWidget_JoinGame::HandleJoinSessionFailed(FString Reason, FTogetherSession SessionState)
{
	Debug::Print("HandleJoinSessionFailed");

	PushErrorMessage(EErrorType::JoinFailed, Reason);
}

void UWidget_JoinGame::HandleJoinedGameSession(FString SessionId)
{
	DeactivateWidget();
}

void UWidget_JoinGame::HandleGameSessionSelected(FTogetherSessionItem SelectedSession)
{
	Debug::Print(SelectedSession.SessionId);

	if (SelectedSession.SessionId.IsEmpty())
	{
		PushErrorMessage(EErrorType::SelectionError);
	}
	else if (MP)
	{
		MP->JoinGame(SelectedSession, TravelDelay);
		bIsJoiningGame = true;
		bShouldJoinGame = false;
		UpdateViewState();
	}
}

void UWidget_JoinGame::UpdateViewState(const bool bIsDeactivate) const
{
	SetNoResultsState();
	SetListViewState(bIsDeactivate);
	SetTitleState(bIsDeactivate);
	SetProgressBarState();
	SetSessionsListState(bIsDeactivate);
	SetIconState();
}

void UWidget_JoinGame::PushErrorMessage(const EErrorType ErrorType, const FString& Reason) const
{
	if (!UI)
	{
		Debug::Print("Unable to display error: UI subsystem is unavailable");
		return;
	}

	const FGameplayTag IconTag = FGameplayTag::RequestGameplayTag(TEXT("UI.Icons.Info"));
	const FGameplayTag SoundFXTag = FGameplayTag::RequestGameplayTag(TEXT("UI.SoundFX.State.Error"));
	FConfirmationButtonLabels ButtonLabels = FConfirmationButtonLabels();
	ButtonLabels.YesLabel = FText::FromString("Refresh");

	FText ScreenTitle;
	FText DefaultMessage;
	switch (ErrorType)
	{
		case EErrorType::JoinFailed:
			ScreenTitle = NSLOCTEXT("JoinGame", "JoinFailedTitle", "Unable to join the game");
			DefaultMessage = NSLOCTEXT(
				"JoinGame",
				"JoinFailedMessage",
				"The selected game could not be joined. Refresh the game list and try again.");
			break;

		case EErrorType::FindFailed:
			ScreenTitle = NSLOCTEXT("JoinGame", "FindFailedTitle", "Unable to find online games");
			DefaultMessage = NSLOCTEXT(
				"JoinGame",
				"FindFailedMessage",
				"The game search could not be completed. Refresh the game list and try again.");
			break;

		case EErrorType::SelectionError:
			ScreenTitle = NSLOCTEXT("JoinGame", "SelectionErrorTitle", "Unable to select the game");
			DefaultMessage = NSLOCTEXT(
				"JoinGame",
				"SelectionErrorMessage",
				"The selected game is not valid. Refresh the game list and try again.");
			break;
	}

	UI->PushConfirmationScreenToStack(
		EConfirmationScreenType::Ok,
		ScreenTitle,
		DefaultMessage,
		ButtonLabels,
		[](EConfirmationButtonType) {},
		IconTag,
		SoundFXTag);
}

void UWidget_JoinGame::SetListViewState(const bool bIsDeactivate) const
{
	if (!ContentView)
	{
		return;
	}

	// in deactivation, reset scroll amount
	if (bIsDeactivate)
	{
		ContentView->SetVerticalScrollOffset(0.0f);
		ContentView->SetCollapsed(true);
		return;
	}

	if (!bInitialized)
	{
		ContentView->SetCollapsed(true, true);
		return;
	}

	if (bIsJoiningGame)
	{
		ContentView->SetCollapsed(true);
		return;
	}

	if (!bIsSearching)
	{
		ContentView->SetCollapsed(false);
		return;
	}

	if (bIsSearching)
	{
		ContentView->SetCollapsed(true);
		return;
	}
}

void UWidget_JoinGame::SetTitleState(const bool bIsDeactivate) const
{
	// protect for title
	if (!Title)
	{
		return;
	}

	// initialize to searching
	if (!bInitialized)
	{
		SetSpacerState(true);
		Title->SetProgressText(FindingSessions);
		Title->SetWorking();
		Title->SetCollapsed(false);
		return;
	}

	// when joining a game, collapse list view
	if (bIsJoiningGame)
	{
		SetSpacerState(true);
		Title->SetProgressText(JoiningGame);
		Title->SetWorking();
		Title->SetCollapsed(false);
		return;
	}

	// has searched, found no results, display no results w/o a title
	// if deactivating don't do anything to the title
	if (!bIsSearching && !bHasResults)
	{
		SetSpacerState(false);
		if (!bIsDeactivate)
		{
			Title->SetIdle();
			Title->SetCollapsed(true);
			return;
		}
	}

	// has searched, found results
	if (!bIsSearching && bHasResults)
	{
		SetSpacerState(true);
		Title->SetDefaultText(PickSession);
		Title->SetIdle();
		Title->SetCollapsed(false);
		return;
	}

	// Is actively searching
	if (bIsSearching)
	{
		SetSpacerState(true);
		Title->SetDefaultText(FindingSessions);
		Title->SetWorking();
		Title->SetCollapsed(false);
		return;
	}
}

void UWidget_JoinGame::SetNoResultsState() const
{

	// protect for title
	if (!NoResults || !SessionsList)
	{
		return;
	}

	if (!bInitialized || bIsSearching || bIsJoiningGame)
	{
		Debug::Print("Has Results");
		NoResults->SetVisibility(ESlateVisibility::Collapsed);
		SessionsList->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if (!bHasResults)
	{
		Debug::Print("No Results");
		NoResults->SetVisibility(ESlateVisibility::Visible);
		SessionsList->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

}

void UWidget_JoinGame::SetSessionsListState(const bool bIsDeactivate) const
{
	if (bIsDeactivate && SessionsList)
	{
		SessionsList->DeactivateListView();
	}
}

void UWidget_JoinGame::FadeChrome(const bool bFade)
{
	SetHeaderShouldFade(bFade);
	SetFooterShouldFade(bFade);
}

void UWidget_JoinGame::SetIconState() const
{
	if (!TitleIcon)
	{
		return;
	}
	else
	{
		TitleIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UWidget_JoinGame::SetSpacerState(const bool bShow) const
{
	if (Spacer)
	{
		Spacer->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UWidget_JoinGame::SetProgressBarState() const
{
	if (!ProgressBar)
	{
		return;
	}
	if (bIsSearching || bIsJoiningGame)
	{
		ProgressBar->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}
