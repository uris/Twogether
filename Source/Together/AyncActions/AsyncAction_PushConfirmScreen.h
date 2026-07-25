// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SharedTypes/SharedTypes.h"
#include "Widgets/Screens/Widget_Confirmation.h"
#include "AsyncAction_PushConfirmScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate,
                                            EConfirmationButtonType,
                                            ClickedButtonType);

/**
 *
 */
UCLASS()
class TOGETHER_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

protected:
	virtual void Activate() override;

public:
	UFUNCTION(BlueprintCallable,
		meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true",
			DisplayName = "Push Confirmation Widget To Widget Stack"),
		Category="Async Actions")
	static UAsyncAction_PushConfirmScreen* PushConfirmationWidget(const UObject* WorldContextObject,
	                                                              EConfirmationScreenType ScreenType,
	                                                              FText InScreenTitle,
	                                                              FText InScreenMessage,
	                                                              FConfirmationButtonLabels InButtonLabels,
	                                                              UPARAM(meta=(Categories="UI.Icon"))
	                                                              FGameplayTag IconTag,
	                                                              UPARAM(meta=(Categories="UI.SoundFX"))
	                                                              FGameplayTag SoundFXTag);

	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnButtonClicked;

private:
	TWeakObjectPtr<UWorld> CachedWorld;
	EConfirmationScreenType CachedScreenType;
	FText CachedScreenTitle;
	FText CachedScreenMessage;
	FConfirmationButtonLabels CachedButtonLabels;
	FGameplayTag CachedIconTag;
	FGameplayTag CachedSoundFXTag;
};
