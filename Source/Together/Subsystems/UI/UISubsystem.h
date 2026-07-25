// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "SharedTypes/SharedTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/UISoundFXs.h"
#include "Widgets/Screens/Widget_Confirmation.h"
#include "Widgets/Base/Widget_PrimaryLayout.h"
#include "UISubsystem.generated.h"

struct FStreamableHandle;
class UUICommonButtonBase;
class UWidget_ActivatableBase;

enum class EAsyncPushWidgetState : uint8
{
	OnBeforePush,
	OnAfterPush,
};

// notify description text of the button when hovered
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionUpdateDelegate,
                                             UUICommonButtonBase*,
                                             Button,
                                             const FText&,
                                             Description);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHelpTextUpdateDelegate,
                                             UObject*,
                                             Object,
                                             const FText&,
                                             HelpText);

// notify core sounds have been loaded
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoreUISoundFXLoaded, int32, TotalSFXToLoad, int32, SFXLoaded);

/**
 *
 */
UCLASS()
class TOGETHER_API UUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// UI helper to return current input mode that is active
	UFUNCTION(BlueprintCallable, Category = "UI")
	static ECommonInputType GetCurrentInputMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI")
	USoundBase* GetPreloadedSFX(FGameplayTag SoundFXTag) const;

	// UI helper to return current input mode that is active
	UFUNCTION(BlueprintCallable, Category = "UI")
	static bool IsGamepadInputMode(const UObject* WorldContextObject);

	// Notifier of description updates
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionUpdateDelegate OnButtonDescriptionUpdated;

	// Notifier of help text updates
	UPROPERTY(BlueprintAssignable)
	FOnHelpTextUpdateDelegate OnHelpTextUpdated;

	// Notifier of core sounds loaded
	UPROPERTY(BlueprintAssignable)
	FOnCoreUISoundFXLoaded OnCoreSoundFXLoaded;

	// get subsystem util for c++
	UFUNCTION(BlueprintCallable, Category="UI")
	static UUISubsystem* GetUISubsystem(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);

	// UI helper to quit game resetting the input mode when UI triggers quit
	UFUNCTION(BlueprintCallable, Category = "UI")
	static void ResetInputAndQuitGame(UObject* ObjectContext);

	// push a new widget to the appropriate widget stack slot
	void PushSoftWidgetToStack(const FGameplayTag& InWidgetStackTag,
	                           TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	                           TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)>
	                           AsyncPushStateCallback) const;

	// push a new confirmation widget to the modal widget stack slot
	void PushConfirmationScreenToStack(const EConfirmationScreenType& InScreenType,
	                                   const FText& InScreenTitle,
	                                   const FText& InScreenMessage,
	                                   const FConfirmationButtonLabels& InButtonLabels,
	                                   TFunction<void(EConfirmationButtonType)> ButtonClickedCallback,
	                                   const FGameplayTag& InIconTag = FGameplayTag(),
	                                   const FGameplayTag& InSoundFXTag = FGameplayTag()) const;

	// get widget satck by tag name
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& WidgetStackTag) const;

protected:
	// begin interface subsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// init subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// de-init subsystem
	virtual void Deinitialize() override;

	// core UI SFX to preload
	UPROPERTY(EditDefaultsOnly, Category = "UI|Audio", meta = (Categories = "UI.SoundFX.Button"))
	TArray<FGameplayTag> SoundFXToPreload;

	bool AreCoreSoundsLoaded() const
	{
		return bCoreSoundFXLoaded;
	};

private:
	UPROPERTY(transient)
	UWidget_PrimaryLayout* PrimaryLayout;

	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<USoundBase>> PreloadedSoundFX;

	UPROPERTY()
	TObjectPtr<UUISoundFXs> LoadedSoundFXLibrary;

	// Streamable handle for loading sound library
	TSharedPtr<FStreamableHandle> StreamableHandle;

	// track core sounds FX load state
	bool bSFXLibraryLoaded = false;

	// track core sounds FX load state
	bool bCoreSoundFXLoaded = false;

	// handle starting and completion of core sound load
	void BeginLoadingCoreSFX();
	void HandleCoreSFXLoaded();
	void SoundFXLibraryDidLoad();

	// helper: construct the lambda function when pushing a soft widget to the UI widget stack
	void PushSoftWidgetToStackLambda(const FGameplayTag& InWidgetStackTag,
	                                 const TSoftClassPtr<UWidget_ActivatableBase>& InSoftWidgetClass,
	                                 TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)>
	                                 AsyncPushStateCallback) const;

};
