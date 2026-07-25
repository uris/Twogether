// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Base/Widget_ContentViewBase.h"
#include "Widget_JoinGame.generated.h"

class UUIProgressSimple;
class UUICommonTextBase;
class USpacer;
class UImage;
class UMultiplayerSubsystem;
struct FTogetherSessionItem;
class UUICommonListViewSessionsBase;
class UUICommonProgressText;

UENUM()
enum class EErrorType : uint8
{
	JoinFailed UMETA(DisplayName = "Join Failed"),
	FindFailed UMETA(DisplayName = "Find Failed"),
	SelectionError UMETA(DisplayName = "Selection Error"),
};

/**
 *
 */
UCLASS()
class TOGETHER_API UWidget_JoinGame : public UWidget_ContentViewBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* TitleIcon;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UUICommonProgressText* Title;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	USizeBox* SizeBox;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UUICommonListViewSessionsBase* SessionsList;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UUICommonTextBase* NoResults;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	USpacer* Spacer;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	UUIProgressSimple* ProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Travel Types")
	float TravelDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
	FString FindingSessions = TEXT("Finding games");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
	FString PickSession = TEXT("Pick a game");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Content")
	FString JoiningGame = TEXT("Joining Game");

	UFUNCTION(BlueprintCallable)
	void HandleFindGamesComplete(const TArray<FTogetherSessionItem>& FoundSessions);

	UFUNCTION(BlueprintCallable)
	void HandleFindGamesFailed(FString Reason, FTogetherSession SessionState);

	UFUNCTION(BlueprintCallable)
	void HandleJoinSessionFailed(FString Reason, FTogetherSession SessionState);

	UFUNCTION(BlueprintCallable)
	void HandleJoinedGameSession(FString SessionId);

	UFUNCTION(BlueprintCallable)
	void HandleGameSessionSelected(FTogetherSessionItem SelectedSession);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual void PrepareForExit(EWidgetExitType Type) override;

private:
	void UpdateViewState(bool bIsDeactivate = false) const;

	void PushErrorMessage(EErrorType ErrorType, const FString& Reason = FString()) const;

	void SetListViewState(bool bIsDeactivate = false) const;

	void SetTitleState(bool bIsDeactivate = false) const;

	void SetNoResultsState() const;

	void SetSessionsListState(bool bIsDeactivate) const;

	void FadeChrome(bool bFade);

	void SetIconState() const;

	void SetSpacerState(bool bShow) const;
	void SetProgressBarState() const;

	bool bInitialized = false;

	bool bHasResults = false;

	bool bIsSearching = true;

	bool bShouldJoinGame = false;

	bool bIsJoiningGame = false;

	TArray<FTogetherSessionItem> CurrentGameSessions;

	UPROPERTY()
	TObjectPtr<UMultiplayerSubsystem> MP = nullptr;

};
