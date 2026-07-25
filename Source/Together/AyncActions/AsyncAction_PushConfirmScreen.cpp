// Uris - All Rights Reserved


#include "AyncActions/AsyncAction_PushConfirmScreen.h"

#include "Engine/Engine.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Utility/Debug.h"

void UAsyncAction_PushConfirmScreen::Activate()
{
	Super::Activate();

	// get UI Subsystem
	UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(CachedWorld.Get());

	// use subsystem to push the modal
	UISubsystem->PushConfirmationScreenToStack(CachedScreenType,
	                                           CachedScreenTitle,
	                                           CachedScreenMessage,
	                                           CachedButtonLabels,
	                                           [this](EConfirmationButtonType ClickedButtonType)
	                                           {
		                                           OnButtonClicked.Broadcast(ClickedButtonType);
		                                           SetReadyToDestroy();
	                                           },
	                                           CachedIconTag,
	                                           CachedSoundFXTag);
}

UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmationWidget(
	const UObject* WorldContextObject,
	EConfirmationScreenType ScreenType,
	FText InScreenTitle,
	FText InScreenMessage,
	FConfirmationButtonLabels InButtonLabels,
	FGameplayTag IconTag,
	FGameplayTag SoundFXTag)
{
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			UAsyncAction_PushConfirmScreen* Node = NewObject<UAsyncAction_PushConfirmScreen>();
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMessage = InScreenMessage;
			Node->CachedScreenType = ScreenType;
			Node->CachedButtonLabels = InButtonLabels;
			Node->CachedIconTag = IconTag;
			Node->CachedWorld = World;
			Node->CachedSoundFXTag = SoundFXTag;

			Node->RegisterWithGameInstance(World);

			return Node;
		}

	}

	return nullptr;
}
