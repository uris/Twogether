// Uris - All Rights Reserved


#include "AyncActions/AsyncAction_PushSoftWidget.h"

#include "Engine/Engine.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Widgets/Base/Widget_ActivatableBase.h"

void UAsyncAction_PushSoftWidget::Activate()
{
	Super::Activate();

	const UUISubsystem* UISubsystem = UUISubsystem::GetUISubsystem(CachedOwningWorld.Get());

	const TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> Callback = [this](
		const EAsyncPushWidgetState InPushState,
		UWidget_ActivatableBase* InPushedWidget)
	{
		switch (InPushState)
		{
			case EAsyncPushWidgetState::OnBeforePush:
				InPushedWidget->SetOwningPlayer(CachedOwningPC.Get());
				OnBeforePush.Broadcast(InPushedWidget);
				break;
			case EAsyncPushWidgetState::OnAfterPush:
				OnAfterPush.Broadcast(InPushedWidget);
				if (bCachedFocusOnNewlyPushedWidget)
				{
					UWidget* WidgetToFocus = InPushedWidget->GetDesiredFocusTarget();
					if (WidgetToFocus)
					{
						WidgetToFocus->SetFocus();
					}
				}
				SetReadyToDestroy();
				break;
			default:
				break;
		}
	};

	UISubsystem->PushSoftWidgetToStack(CachedWidgetStackTag, CachedSoftWidgetClass, Callback);
}

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject,
                                                                         APlayerController* OwningPlayerController,
                                                                         TSoftClassPtr<UWidget_ActivatableBase>
                                                                         InSoftWidgetClass,
                                                                         FGameplayTag InWidgetStackTag,
                                                                         bool bFocusOnNewlyPushedWidget)
{
	checkf(!InSoftWidgetClass.IsNull(), TEXT("Was passed a null soft widget class"));
	if (GEngine)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}

	return nullptr;
}