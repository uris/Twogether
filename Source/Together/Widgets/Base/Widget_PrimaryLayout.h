// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;
/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class TOGETHER_API UWidget_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/** get widget stack by game play tag */
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InStackTag) const;

protected:
	/** show/hide debugging */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bShowDebugMessages = false;

	/** Register Widget Stack Tags to Widget Stack */
	UFUNCTION(BlueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "UI.WidgetStack"))
	                         FGameplayTag InStackTag,
	                         UCommonActivatableWidgetContainerBase* InWidgetStack);

private:
	/** Widget Stack Array */
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> WidgetStack;

	/** debug helper */
	void Print(const FString& Message) const;

};
