// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "AsyncAction_PushSoftWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPushSoftWidgetDelegate,
	UWidget_ActivatableBase*,
	PushedWidget
	);

/**
 *
 */
UCLASS()
class TOGETHER_API UAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

protected:
	virtual void Activate() override;

public:
	UFUNCTION(BlueprintCallable,
		meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true",
			DisplayName = "Push Soft Widget To Widget Stack"),
		Category="Async Actions")
	static UAsyncAction_PushSoftWidget* PushSoftWidget(const UObject* WorldContextObject,
	                                                   APlayerController* OwningPlayerController,
	                                                   TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	                                                   UPARAM(meta = (Categories = "UI.WidgetStack"))
	                                                   FGameplayTag InWidgetStackTag,
	                                                   bool bFocusOnNewlyPushedWidget = true);

	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnBeforePush;

	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnAfterPush;

private:
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	TWeakObjectPtr<APlayerController> CachedOwningPC;
	TSoftClassPtr<UWidget_ActivatableBase> CachedSoftWidgetClass;
	FGameplayTag CachedWidgetStackTag;
	bool bCachedFocusOnNewlyPushedWidget = false;

};