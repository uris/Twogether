// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Base/Widget_ContentViewBase.h"
#include "Subsystems/Multiplayer/MultiplayerSubsystem.h"
#include "Widget_CreateGame.generated.h"

class USpacer;
class UUICommonButtonBase;
class UUICommonProgressText;
class UUICommonInputFieldBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UWidget_CreateGame : public UWidget_ContentViewBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUICommonInputFieldBase> Input_GameName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UUICommonButtonBase> Btn_Create;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	USpacer* Spacer;

	UPROPERTY(BlueprintReadOnly)
	UMultiplayerSubsystem* MP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CreateDelay = 3.0f;

	UFUNCTION()
	void HandleOnServerCreated(FTogetherSession SessionState);

	UFUNCTION()
	void HandleOnCreateServerFail(FString Reason, FTogetherSession SessionState) const;

	UFUNCTION()
	void HandleOnSessionActiveChange(bool bActive) const;

	UFUNCTION()
	void HandleButtonClick(const FString& ButtonName, EAsyncButtonState ButtonState);

	UFUNCTION()
	void HandleInputFieldValidationChange(UUICommonInputFieldBase* InputText, bool bDidValidate) const;

	UFUNCTION()
	void HandleContentViewTransitionEnd(bool bCollapsed) const;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual void PrepareForExit(EWidgetExitType Type) override;

private:
	// push error message
	void PushErrorMessage(const FString& Reason) const;

	// manage delayed server creation
	bool bIsCreatingGame = false;

	// handler for delayed click execution
	FTimerHandle TimerHandle;

	// create game server
	void CreateGameServer();

	// helper
	void CollapseContentView(const bool bCollapse);

	// helper
	void ShowProgress(bool bProgressState) const;

	// helper
	void ResetCreateGame();

};
