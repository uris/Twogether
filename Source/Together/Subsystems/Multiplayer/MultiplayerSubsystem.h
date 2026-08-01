// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/TimerHandle.h"

#include "MultiplayerSubsystem.generated.h"

struct FGameplayTag;

UENUM(BlueprintType)
enum class EOnlineProvider : uint8
{
	Null UMETA(DisplayName = "NULL"),
	Steam UMETA(DisplayName = "STEAM"),
	Unknown UMETA(DisplayName = "UNKNOWN"),
};

UENUM(BlueprintType)
enum class ETogetherSessionState : uint8
{
	Creating UMETA(DisplayName = "Creating Session"),
	Destroying UMETA(DisplayName = "Destroying Session"),
	Reconnecting UMETA(DisplayName = "Reconnecting"),
	Finding UMETA(DisplayName = "Finding"),
	Joining UMETA(DisplayName = "Joining"),
	Error UMETA(DisplayName = "Error"),
	Idle UMETA(DisplayName = "Idle"),
};

UENUM(BlueprintType)
enum class ETogetherSessionFailure : uint8
{
	Creating UMETA(DisplayName = "Creating Session"),
	Destroying UMETA(DisplayName = "Destroying Session"),
	Finding UMETA(DisplayName = "Finding"),
	Joining UMETA(DisplayName = "Joining")
};

USTRUCT(BlueprintType)
struct FTogetherSession
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ETogetherSessionState State = ETogetherSessionState::Idle;

	UPROPERTY(BlueprintReadOnly)
	FName GameName = NAME_None;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldReconnect = false;
};

USTRUCT(BlueprintType)
struct FTogetherSessionItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString GameName = FString();

	UPROPERTY(BlueprintReadOnly)
	FString SessionId = FString();

	UPROPERTY(BlueprintReadOnly)
	bool bIsFull = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCreateServerFailed,
                                             FString,
                                             Reason,
                                             FTogetherSession,
                                             SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDestroyServerFailed,
                                             FString,
                                             Reason,
                                             FTogetherSession,
                                             SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsFailed,
                                             FString,
                                             Reason,
                                             FTogetherSession,
                                             SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinSessionFailed,
                                             FString,
                                             Reason,
                                             FTogetherSession,
                                             SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSessionsFound, const TArray<FTogetherSessionItem>&, FoundSessions);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerCreated, FTogetherSession, SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnServerDestroyed, FTogetherSession, SessionState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinedGameSession, FString, SessionId);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionActiveChange, bool, Active);

/**
 *
 */
UCLASS()
class TOGETHER_API UMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void HostGame(const FString& GameName);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void JoinGame(const FTogetherSessionItem& TogetherGameSession,
	              UPARAM(meta=(ClampMin="0.0", UIMin="0.0", Units="s"))
	              float TravelDelay = 0.0f);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void CancelJoinGame();

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void DestroySession(const bool bShouldReconnect);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void FindGameSessions();

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	static EOnlineProvider GetOnlineProvider(const UWorld* World);
	static FString GetOnlineProviderName(const UWorld* World);

	UFUNCTION(BlueprintCallable, Category="Multiplayer|Functions")
	void FindGameSessionByGameName(const FString& GameName);

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnJoinSessionFailed OnJoinSessionFailed;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnCreateServerFailed OnCreateServerFail;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnDestroyServerFailed OnDestroyServerFail;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnFindSessionsFailed OnFindSessionsFail;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnGameSessionsFound OnGameSessionsFound;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnServerCreated OnServerCreated;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnServerDestroyed OnServerDestroyed;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnSessionActiveChange OnSessionActiveChange;

	UPROPERTY(BlueprintAssignable, Category="Multiplayer|Events")
	FOnJoinedGameSession OnJoinedGameSession;

protected:
	UMultiplayerSubsystem();

	// override initialize
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// override de-initialize
	virtual void Deinitialize() override;

private:
	// hold pointer to session interface
	IOnlineSessionPtr SessionInterface;

	// Handler for the created session delegate
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// Handler for the destroyed session delegate
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// handler for found sessions
	void OnFindSessionsComplete(bool bWasSuccessful);

	// handler for joined session
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// travel to lobby for users joining a server
	void TravelToResolvedAddress(const FString& ResolvedAddress);

	// Shared implementation for unfiltered and game-name-filtered searches
	void FindGameSessionsInternal(const TOptional<FString>& GameNameFilter);

	// helper: reset session state back to initial state
	void ResetSessionState();

	// helper: register/release needed delegate handlers - bool returns success state
	void RegisterDelegateHandlers();
	void ReleaseDelegateHandlers() const;

	// helper: handle and communicate failures
	void BroadcastSessionFailure(const FString& Reason, ETogetherSessionFailure Type);

	// helper: check for active request / response
	bool IsActiveGuard() const;

	// register / deregister utility for handlers
	FDelegateHandle OnCreateSessionCompleteHandler;
	FDelegateHandle OnDestroySessionCompleteHandler;
	FDelegateHandle OnFindSessionsCompleteHandler;
	FDelegateHandle OnJoinSessionCompleteHandler;

	// transient: track and share session state
	FTogetherSession TogetherSessionState;

	// transient: store session search
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// transient: store join search result that is pending
	FOnlineSessionSearchResult JoinSessionResult;

	// transient: delay and timer used only after a successful session join
	float PendingTravelDelay = 0.0f;
	FTimerHandle PendingTravelTimerHandle;
	bool bHasReplacementJoinRequest = false;
	FTogetherSessionItem ReplacementJoinRequest;
	float ReplacementJoinTravelDelay = 0.0f;
	bool bJoinCancellationRequested = false;

	// transient: optional display-name filter for the active search
	TOptional<FString> SessionSearchGameNameFilter;

	// server name
	const FName ServerName = FName("TWOGETHER_SERVER");

	// helper: enum to string
	static FString JoinResultToString(EOnJoinSessionCompleteResult::Type JoinResult);

	// test game sessions
	static TArray<FTogetherSessionItem> MakeTestSessions(TArray<FTogetherSessionItem> RealEntries);
};
