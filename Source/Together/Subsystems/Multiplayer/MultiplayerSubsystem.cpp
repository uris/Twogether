// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSubsystem.h"

#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"
#include "Utility/Debug.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "Settings/TogetherSettings.h"
#include "TimerManager.h"
#include "Utility/AutoGen.h"

UMultiplayerSubsystem::UMultiplayerSubsystem()
{
	Debug::Print(TEXT("MSS Constructed"));
}

void UMultiplayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// init debug
	const FString Message = FString::Printf(TEXT("Multiplayer Subsystem Initialized"));
	Debug::Print(Message);

	// get the online subsystem
	if (const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		const FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		Debug::Print(FString::Printf(TEXT("MSS Online Subsystem: %s"), *SubsystemName));

		// create session interface
		SessionInterface = OnlineSubsystem->GetSessionInterface();

		// register online system delegate handlers
		RegisterDelegateHandlers();
	}

	// make sure session state is reset
	ResetSessionState();
}

void UMultiplayerSubsystem::Deinitialize()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PendingTravelTimerHandle);
	}

	// clear delegate registration
	ReleaseDelegateHandlers();

	// reset state
	ResetSessionState();

	// reset interface -> call last before super
	SessionInterface.Reset();

	// call parent
	Super::Deinitialize();

	UE_LOG(LogTemp, Warning, TEXT("MSS deinitialized"));
}

void UMultiplayerSubsystem::HostGame(const FString& GameName)
{
	const FString Message = FString::Printf(TEXT("MSS Host Game. User Facing Name: %s"), *GameName);
	Debug::Print(Message);

	// protect for repeat calls
	if (IsActiveGuard())
	{
		return;
	}

	if (GameName.IsEmpty())
	{
		const FString Reason = "MSS CreateServer: Invalid Game Name";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Creating);
		return;
	}

	if (!SessionInterface.IsValid())
	{
		const FString Reason = "MSS CreateServer: Session Interface Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Creating);
		return;
	}

	const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (!OnlineSubsystem)
	{
		const FString Reason = "MSS CreateServer: Online Subsystem Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Creating);
		return;
	}

	// update session state to creating once checks pass
	TogetherSessionState.State = ETogetherSessionState::Creating;
	TogetherSessionState.GameName = FName(GameName);
	TogetherSessionState.bShouldReconnect = false;

	// broadcast active
	OnSessionActiveChange.Broadcast(true);

	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		const FString ExistsMessage = FString::Printf(
			TEXT("MSS CreateServer: Session %d already exists:"),
			NAME_GameSession);
		Debug::Print(ExistsMessage);
		DestroySession(true);
		return;
	}

	// create session settings
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	const bool bIsLan = OnlineSubsystem->GetSubsystemName() == FName(TEXT("NULL"));
	SessionSettings.bIsLANMatch = bIsLan;
	// use the user game name as the display name for other to find the game
	SessionSettings.Set(ServerName,
	                    GameName,
	                    EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// call to request the creation of the session
	const bool bDidRequestCreate = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);

	// handle request failure
	if (!bDidRequestCreate)
	{
		const FString Reason = TEXT("Unable to request session creations");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Creating);
	}

}

/**
 * Note: "server" join and "user" join are two different concepts. For users, selecting "join" can
 * mean listing available games to potentially join: from an implementation perspective this is find
 * sessions. Joining from an implementation perspective is selecting a specific session to travel to.
 */
void UMultiplayerSubsystem::JoinGame(const FTogetherSessionItem& TogetherGameSession, const float TravelDelay)
{
	// A session join has completed, but its travel is still delayed. Leave that
	// online session first, then restart using the newest requested session.
	if (UWorld* World = GetWorld();
		World && World->GetTimerManager().IsTimerActive(PendingTravelTimerHandle))
	{
		World->GetTimerManager().ClearTimer(PendingTravelTimerHandle);
		PendingTravelTimerHandle.Invalidate();
		PendingTravelDelay = 0.0f;
		bHasReplacementJoinRequest = true;
		ReplacementJoinRequest = TogetherGameSession;
		ReplacementJoinTravelDelay = FMath::Max(0.0f, TravelDelay);

		// Allow the existing destroy workflow to cleanly leave the joined session.
		TogetherSessionState.State = ETogetherSessionState::Idle;
		DestroySession(false);
		return;
	}

	// If cleanup for a replacement is already running, retain only the newest
	// request and let OnDestroySessionComplete start it.
	if (bHasReplacementJoinRequest && TogetherSessionState.State == ETogetherSessionState::Destroying)
	{
		ReplacementJoinRequest = TogetherGameSession;
		ReplacementJoinTravelDelay = FMath::Max(0.0f, TravelDelay);
		return;
	}

	// protect for repeat calls
	if (IsActiveGuard())
	{
		return;
	}

	// must have a valid together game id session
	if (TogetherGameSession.SessionId.IsEmpty())
	{
		const FString Reason = "MSS JoinServer: Invalid Game Reference";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// session interface must be valid
	if (!SessionInterface.IsValid())
	{
		const FString Reason = "MSS JoinServer: Session Interface Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// session results must exist
	// TODO: harden to search for results if none exist?
	if (!SessionSearch.IsValid() || SessionSearch->SearchResults.IsEmpty())
	{
		const FString Reason = "MSS JoinServer: No search results";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// update session state to joining
	TogetherSessionState.State = ETogetherSessionState::Joining;
	TogetherSessionState.bShouldReconnect = false;
	PendingTravelDelay = FMath::Max(0.0f, TravelDelay);

	// empty out cached join session search result
	JoinSessionResult = FOnlineSessionSearchResult();

	// broadcast active change
	OnSessionActiveChange.Broadcast(true);

	// debug join
	const FString Message = FString::Printf(
		TEXT("MSS Join Game. Session ID: %s, Game Name: %s"),
		*TogetherGameSession.SessionId,
		*TogetherGameSession.GameName);
	Debug::Print(Message);

	// Loop through all found servers
	for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
	{
		// match sessions by id

		// if matching send to travel to map helper and exit loop
		if (const FString SessionId = SearchResult.Session.GetSessionIdStr(); TogetherGameSession.SessionId.
			Equals(SessionId))
		{
			JoinSessionResult = SearchResult;
			Debug::Print(FString::Printf(TEXT("Joining Session: %s"), *SessionId));
			break;
		}
	}

	// handle no matches
	if (!JoinSessionResult.IsValid())
	{
		const FString Reason = "MSS JoinServer: Unable to match requested session";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// call join as client
	bool bDidRequestJoin = SessionInterface->JoinSession(0, NAME_GameSession, JoinSessionResult);

	// validate call was successful
	if (!bDidRequestJoin)
	{
		const FString Reason = "MSS JoinServer: Unable to call join session";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}
}

void UMultiplayerSubsystem::CancelJoinGame()
{
	const bool bJoinRequestPending = TogetherSessionState.State == ETogetherSessionState::Joining
	                                 && !PendingTravelTimerHandle.IsValid();
	const bool bJoinedAwaitingTravel = PendingTravelTimerHandle.IsValid();
	const bool bReplacementCleanupActive = bHasReplacementJoinRequest
	                                       && TogetherSessionState.State == ETogetherSessionState::Destroying;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PendingTravelTimerHandle);
	}
	PendingTravelTimerHandle.Invalidate();
	PendingTravelDelay = 0.0f;
	bHasReplacementJoinRequest = false;
	ReplacementJoinRequest = FTogetherSessionItem();
	ReplacementJoinTravelDelay = 0.0f;
	JoinSessionResult = FOnlineSessionSearchResult();
	SessionSearch.Reset();
	SessionSearchGameNameFilter.Reset();

	// The online API has no cancellable JoinSession request. Mark it cancelled
	// so a late completion can leave the joined session without travelling.
	if (bJoinRequestPending)
	{
		bJoinCancellationRequested = true;
		TogetherSessionState.State = ETogetherSessionState::Idle;
		TogetherSessionState.bShouldReconnect = false;
		OnSessionActiveChange.Broadcast(false);
		return;
	}

	bJoinCancellationRequested = false;

	// JoinSession has completed during the travel delay, so leave that online
	// session cleanly. Existing destroy completion will reset state to idle.
	if (bJoinedAwaitingTravel && SessionInterface.IsValid())
	{
		TogetherSessionState.State = ETogetherSessionState::Idle;
		DestroySession(false);
		return;
	}

	// A replacement request was already leaving the previous session. Let that
	// destroy finish, but the queued replacement above has been discarded.
	if (bReplacementCleanupActive)
	{
		return;
	}

	ResetSessionState();
}

void UMultiplayerSubsystem::OnCreateSessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	const FString Message = FString::Printf(
		TEXT("Game session created for %s with session %s"),
		*TogetherSessionState.GameName.ToString(),
		*SessionName.ToString());
	Debug::Print(Message);

	if (bWasSuccessful)
	{
		// update session state
		TogetherSessionState.State = ETogetherSessionState::Idle;
		TogetherSessionState.bShouldReconnect = false;

		// broadcast success
		OnServerCreated.Broadcast(TogetherSessionState);

		// broadcast not active
		OnSessionActiveChange.Broadcast(false);

		// travel to lobby map as listen server
		const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
		const FString MapPath = Settings->OnServerCreateMap.ToSoftObjectPath().GetLongPackageName();
		if (!MapPath.IsEmpty())
		{
			UGameplayStatics::OpenLevel(this, FName(*MapPath), true, TEXT("Listen"));
		}
	}
	else
	{
		FString Reason = TEXT("Unable to create game session");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Creating);
	}
}

void UMultiplayerSubsystem::DestroySession(const bool bShouldReconnect)
{
	Debug::Print(TEXT("Destroying game session"));

	// protect for repeat calls not coming from create request
	if (IsActiveGuard() && !bShouldReconnect)
	{
		return;
	}

	// protect for valid session interface
	if (!SessionInterface.IsValid())
	{
		const FString Reason = TEXT("DestroySession: Session Interface Invalid");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Destroying);
		return;
	}

	// update session state marking reconnect as true based on in parameter
	TogetherSessionState.State = ETogetherSessionState::Destroying;
	TogetherSessionState.bShouldReconnect = bShouldReconnect;

	// broadcast not active
	OnSessionActiveChange.Broadcast(true);

	// destroy request
	const bool bDidRequestDestroy = SessionInterface->DestroySession(NAME_GameSession);

	// handle request failure
	if (!bDidRequestDestroy)
	{
		const FString Reason = TEXT("Unable to request destroy existing session");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Destroying);
	}
}

void UMultiplayerSubsystem::OnDestroySessionComplete(const FName SessionName, const bool bWasSuccessful)
{
	const FString Message = FString::Printf(
		TEXT("Game session destroyed for %s"),
		*SessionName.ToString());
	Debug::Print(Message);

	// was destroyed (= success)
	if (bWasSuccessful)
	{
		// update session state to idle
		TogetherSessionState.State = ETogetherSessionState::Idle;
		OnServerDestroyed.Broadcast(TogetherSessionState);

		if (bHasReplacementJoinRequest)
		{
			const FTogetherSessionItem RequestedSession = ReplacementJoinRequest;
			const float RequestedTravelDelay = ReplacementJoinTravelDelay;
			bHasReplacementJoinRequest = false;
			ReplacementJoinRequest = FTogetherSessionItem();
			ReplacementJoinTravelDelay = 0.0f;
			TogetherSessionState.bShouldReconnect = false;
			JoinSessionResult = FOnlineSessionSearchResult();
			JoinGame(RequestedSession, RequestedTravelDelay);
			return;
		}

		// check should reconnect to run host game again
		if (TogetherSessionState.bShouldReconnect)
		{
			// call host game with the cached game name provided by the user
			HostGame(TogetherSessionState.GameName.ToString());
		}
		else
		{
			// was a "clean destroy" so simply reset session state
			ResetSessionState();

			// broadcast not active
			OnSessionActiveChange.Broadcast(false);
		}
	}
	// not able to destroy, so don't attempt further reconnection
	else
	{
		// broadcast fail event
		FString Reason = TEXT("Unable to destroy server");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Destroying);
	}
}

void UMultiplayerSubsystem::FindGameSessions()
{
	FindGameSessionsInternal(TOptional<FString>());
}

void UMultiplayerSubsystem::FindGameSessionByGameName(const FString& GameName)
{
	if (GameName.IsEmpty())
	{
		const FString Reason = TEXT("FindGameSessionByGameName: Invalid GameName");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Finding);
		return;
	}

	FindGameSessionsInternal(TOptional<FString>(GameName));
}

void UMultiplayerSubsystem::FindGameSessionsInternal(const TOptional<FString>& GameNameFilter)
{
	Debug::Print(TEXT("Find game sessions"));

	// protect for repeat calls
	if (IsActiveGuard())
	{
		return;
	}

	const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	if (!OnlineSubsystem)
	{
		const FString Reason = "FindGameSessions: Online Subsystem Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Finding);
		return;
	}

	if (!SessionInterface.IsValid())
	{
		const FString Reason = "FindGameSessions: Session Interface Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Finding);
		return;
	}

	// update session state
	TogetherSessionState.State = ETogetherSessionState::Finding;
	SessionSearchGameNameFilter = GameNameFilter;

	// broadcast active
	OnSessionActiveChange.Broadcast(true);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	const bool bIsLan = OnlineSubsystem->GetSubsystemName() == FName(TEXT("NULL"));
	SessionSearch->bIsLanQuery = bIsLan;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	// trigger request to find sessions
	const bool bDidRequestFindSessions = SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

	// handle request failure
	if (!bDidRequestFindSessions)
	{
		SessionSearchGameNameFilter.Reset();
		const FString Reason = TEXT("Unable to find any game sessions");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Finding);
	}
}

void UMultiplayerSubsystem::OnFindSessionsComplete(const bool bWasSuccessful)
{
	if (!SessionSearch.IsValid())
	{
		SessionSearchGameNameFilter.Reset();
		TogetherSessionState.State = ETogetherSessionState::Idle;
		OnSessionActiveChange.Broadcast(false);
		return;
	}

	if (bWasSuccessful)
	{

		// set session state
		TogetherSessionState.State = ETogetherSessionState::Idle;

		// setup processing of search results
		const TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		const int32 NumSessionsFound = SearchResults.Num();
		TArray<FTogetherSessionItem> TogetherGameSessions;

		// debug session found
		FString Message =
			FString::Printf(TEXT("Sessions found: %d"), NumSessionsFound);
		Debug::Print(Message);

		// Loop through all found servers
		for (const FOnlineSessionSearchResult& SearchResult : SearchResults)
		{

			// ensure valid search result
			if (!SearchResult.IsValid())
			{
				continue;
			}

			// store session item info
			FTogetherSessionItem SessionItem;

			// store game name with check
			if (!SearchResult.Session.SessionSettings.Get(ServerName, SessionItem.GameName) ||
			    SessionItem.GameName.IsEmpty())
			{
				continue;
			}

			// store session id
			SessionItem.SessionId = SearchResult.Session.GetSessionIdStr();

			// store is full flag
			SessionItem.bIsFull = SearchResult.Session.NumOpenPublicConnections <= 0;

			// if game name is being used to match and the result matches the game name, save match flag
			const bool bIsMatch = SessionSearchGameNameFilter.IsSet() && SessionItem.GameName.Equals(
				                      SessionSearchGameNameFilter.GetValue(),
				                      ESearchCase::IgnoreCase);

			// cache matching results, or all results if no match was requested
			if (bIsMatch || !SessionSearchGameNameFilter.IsSet())
			{
				TogetherGameSessions.Add(SessionItem);
				Debug::Print(FString::Printf(TEXT("GameName: %s"), *SessionItem.GameName));
			}
		}

		// reset filter
		SessionSearchGameNameFilter.Reset();

		// broadcast results
		OnGameSessionsFound.Broadcast(MakeTestSessions(TogetherGameSessions));

		// broadcast not active
		OnSessionActiveChange.Broadcast(false);
	}
	else
	{
		SessionSearchGameNameFilter.Reset();
		const FString Reason = TEXT("Unable to find game sessions");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Finding);
	}
}

void UMultiplayerSubsystem::OnJoinSessionComplete(const FName SessionName,
                                                  const EOnJoinSessionCompleteResult::Type Result)
{
	if (bJoinCancellationRequested)
	{
		bJoinCancellationRequested = false;
		JoinSessionResult = FOnlineSessionSearchResult();
		TogetherSessionState.State = ETogetherSessionState::Idle;
		TogetherSessionState.bShouldReconnect = false;
		OnSessionActiveChange.Broadcast(false);

		if (Result == EOnJoinSessionCompleteResult::Success && SessionInterface.IsValid())
		{
			DestroySession(false);
		}
		return;
	}

	// Join must be a success value
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		FString FailReason = JoinResultToString(Result);
		const FString Reason = FString::Printf(TEXT("MSS JoinServer: - %s"), *FailReason);
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// session interface must be valid
	if (!SessionInterface.IsValid())
	{
		const FString Reason = "MSS JoinServer: Session Interface Invalid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// try to resolve connection address
	FString ResolvedAddress;
	bool bResolved = SessionInterface->GetResolvedConnectString(NAME_GameSession, ResolvedAddress);

	// exit on failed to resolve
	if (!bResolved)
	{
		// debug
		const FString Reason = TEXT("Unable to resolve join address");
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// The online session is joined immediately. Delay only the player travel.
	if (PendingTravelDelay > KINDA_SMALL_NUMBER)
	{
		if (UWorld* World = GetWorld())
		{
			FTimerDelegate TravelDelegate;
			TravelDelegate.BindWeakLambda(this,
			                              [this, ResolvedAddress]()
			                              {
				                              TravelToResolvedAddress(ResolvedAddress);
			                              });
			World->GetTimerManager().SetTimer(
				PendingTravelTimerHandle,
				TravelDelegate,
				PendingTravelDelay,
				false);
			return;
		}
	}

	TravelToResolvedAddress(ResolvedAddress);

}

void UMultiplayerSubsystem::TravelToResolvedAddress(const FString& ResolvedAddress)
{
	PendingTravelTimerHandle.Invalidate();
	PendingTravelDelay = 0.0f;
	bJoinCancellationRequested = false;
	bHasReplacementJoinRequest = false;
	ReplacementJoinRequest = FTogetherSessionItem();
	ReplacementJoinTravelDelay = 0.0f;

	// get player controller
	APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();

	// safeguard for player controller
	if (!PlayerController)
	{
		const FString Reason = "MSS JoinServer: No player controller";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// safeguard for cached join search result
	if (!JoinSessionResult.IsValid())
	{
		const FString Reason = "MSS JoinServer: Join Search Result Not Valid";
		BroadcastSessionFailure(Reason, ETogetherSessionFailure::Joining);
		return;
	}

	// travel to address
	PlayerController->ClientTravel(ResolvedAddress, ETravelType::TRAVEL_Absolute);

	// set session state
	TogetherSessionState.State = ETogetherSessionState::Idle;
	TogetherSessionState.bShouldReconnect = false;

	// broadcast joined
	OnJoinedGameSession.Broadcast(JoinSessionResult.GetSessionIdStr());

	// broadcast session active
	OnSessionActiveChange.Broadcast(false);

	// reset member variables
	JoinSessionResult = FOnlineSessionSearchResult();
	SessionSearch.Reset();
	SessionSearchGameNameFilter.Reset();

}

void UMultiplayerSubsystem::ResetSessionState()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PendingTravelTimerHandle);
	}
	PendingTravelTimerHandle.Invalidate();
	PendingTravelDelay = 0.0f;

	TogetherSessionState.State = ETogetherSessionState::Idle;
	TogetherSessionState.GameName = NAME_None;
	TogetherSessionState.bShouldReconnect = false;
	SessionSearchGameNameFilter.Reset();
	// broadcast active
	OnSessionActiveChange.Broadcast(false);
}

void UMultiplayerSubsystem::RegisterDelegateHandlers()
{
	if (SessionInterface.IsValid())
	{
		OnCreateSessionCompleteHandler = SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
			this,
			&UMultiplayerSubsystem::OnCreateSessionComplete);

		OnDestroySessionCompleteHandler = SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
			this,
			&UMultiplayerSubsystem::OnDestroySessionComplete);

		OnFindSessionsCompleteHandler = SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this,
			&UMultiplayerSubsystem::OnFindSessionsComplete);
		OnJoinSessionCompleteHandler = SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this,
			&UMultiplayerSubsystem::OnJoinSessionComplete);
	}
}

void UMultiplayerSubsystem::ReleaseDelegateHandlers() const
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->OnCreateSessionCompleteDelegates.Remove(OnCreateSessionCompleteHandler);
		SessionInterface->OnDestroySessionCompleteDelegates.Remove(OnDestroySessionCompleteHandler);
		SessionInterface->OnFindSessionsCompleteDelegates.Remove(OnFindSessionsCompleteHandler);
		SessionInterface->OnJoinSessionCompleteDelegates.Remove(OnJoinSessionCompleteHandler);
	}
}

void UMultiplayerSubsystem::BroadcastSessionFailure(const FString& Reason, const ETogetherSessionFailure Type)
{
	// set error state
	TogetherSessionState.State = ETogetherSessionState::Error;
	TogetherSessionState.bShouldReconnect = false;

	// debug
	Debug::Print(Reason);

	// perform callback
	switch (Type)
	{
		case ETogetherSessionFailure::Creating:
			OnCreateServerFail.Broadcast(Reason, TogetherSessionState);
			break;
		case ETogetherSessionFailure::Destroying:
			bHasReplacementJoinRequest = false;
			ReplacementJoinRequest = FTogetherSessionItem();
			ReplacementJoinTravelDelay = 0.0f;
			OnDestroyServerFail.Broadcast(Reason, TogetherSessionState);
			break;
		case ETogetherSessionFailure::Finding:
			OnFindSessionsFail.Broadcast(Reason, TogetherSessionState);
			break;
		case ETogetherSessionFailure::Joining:
			if (UWorld* World = GetWorld())
			{
				World->GetTimerManager().ClearTimer(PendingTravelTimerHandle);
			}
			PendingTravelTimerHandle.Invalidate();
			PendingTravelDelay = 0.0f;
			JoinSessionResult = FOnlineSessionSearchResult();
			OnJoinSessionFailed.Broadcast(Reason, TogetherSessionState);
			break;
	}

	// rest back to idle since failure is terminal
	TogetherSessionState.State = ETogetherSessionState::Idle;

	// broadcast active change
	OnSessionActiveChange.Broadcast(false);
}

bool UMultiplayerSubsystem::IsActiveGuard() const
{
	switch (TogetherSessionState.State)
	{
		case ETogetherSessionState::Reconnecting:
		case ETogetherSessionState::Creating:
		case ETogetherSessionState::Destroying:
		case ETogetherSessionState::Finding:
		case ETogetherSessionState::Joining:
		{
			const FString Reason = "Requests/Responses are currently active";
			Debug::Print(Reason);
			return true;
		}
		default:
		{
			return false;
		}
	}
}

FString UMultiplayerSubsystem::JoinResultToString(const EOnJoinSessionCompleteResult::Type JoinResult)
{
	switch (JoinResult)
	{
		case EOnJoinSessionCompleteResult::Success:
			return TEXT("Success");

		case EOnJoinSessionCompleteResult::SessionIsFull:
			return TEXT("Session Is Full");

		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			return TEXT("Session Does Not Exist");

		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			return TEXT("Could Not Retrieve Address");

		case EOnJoinSessionCompleteResult::AlreadyInSession:
			return TEXT("Already In Session");

		case EOnJoinSessionCompleteResult::UnknownError:
		default:
			return TEXT("Unknown Error");
	}
}

TArray<FTogetherSessionItem> UMultiplayerSubsystem::MakeTestSessions(TArray<FTogetherSessionItem> RealEntries)
{

	if (const bool bListFakeGameSessions = GetDefault<UTogetherSettings>()->bListFakeGameSessions; !
		bListFakeGameSessions)
	{
		return RealEntries;
	}

	FTogetherSessionItem Item;

	// create for loop
	for (int i = 0; i < 50; ++i)
	{
		FText RandomGameName = UAutoGen::GenerateRandomWords(TEXT("FunnyWords"));
		Item.bIsFull = false;
		Item.SessionId = TEXT("000-000-000-000");
		Item.GameName = RandomGameName.IsEmpty() ? TEXT("Game name") : RandomGameName.ToString();
		RealEntries.Add(Item);
	}

	return RealEntries;
}

EOnlineProvider UMultiplayerSubsystem::GetOnlineProvider(const UWorld* World)
{
	if (GEngine && World)
	{
		const IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
		const FString Name = OnlineSubsystem->GetSubsystemName().ToString();
		if (Name.Equals("STEAM", ESearchCase::IgnoreCase))
		{
			return EOnlineProvider::Steam;
		}
		if (Name.Equals("NULL", ESearchCase::IgnoreCase))
		{
			return EOnlineProvider::Null;
		}
		return EOnlineProvider::Unknown;
	}
	return EOnlineProvider::Unknown;
}

FString UMultiplayerSubsystem::GetOnlineProviderName(const UWorld* World)
{
	switch (GetOnlineProvider(World))
	{
		case EOnlineProvider::Steam:
			return TEXT("Steam");
		case EOnlineProvider::Null:
			return TEXT("Local");
		default:
			return TEXT("Unknown");
	}
}
