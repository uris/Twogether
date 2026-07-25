// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "SharedTypes/SharedTypes.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "Widget_MainMenu.generated.h"

class UUICommonBoundActionBarBase;
class UHorizontalBox;
class UUICommonButtonBase;

/** Native behavior for the main-menu layout authored by WBP_CAW_MainMenu. */
UCLASS()
class TOGETHER_API UWidget_MainMenu : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual bool NativeOnHandleBackAction() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void OnCreditsRequested();

	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void OnOptionsRequested();

private:
	void HandleCreateClicked() const;
	void HandleCreditsClicked();
	void HandleJoinClicked() const;
	void HandleOptionsClicked() const;
	void HandleQuitClicked();
	void HandleModalStackChanged(UCommonActivatableWidget* DisplayedWidget);
	void HandleMainMenuResumed() const;
	void HandleQuitResult(EConfirmationButtonType Result);
	void PushActivatableWidgetScreen(const FGameplayTag& WidgetTag,
	                                 const FGameplayTag& WidgetStackTag) const;

	UPROPERTY(Transient)
	TObjectPtr<UCommonActivatableWidgetContainerBase> ModalStack;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonBoundActionBarBase> BoundActionBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> Btn_Create;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> Btn_Credits;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> Btn_Join;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> Btn_Options;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> Btn_Quit;

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu|Quit")
	FText QuitTitle = NSLOCTEXT("MainMenu", "QuitTitle", "Quit Game");

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu|Quit", meta = (MultiLine = "true"))
	FText QuitMessage = NSLOCTEXT("MainMenu", "QuitMessage", "Are you sure you want to quit?");
};
