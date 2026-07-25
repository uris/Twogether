// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionButton.h"
#include "UICommonBoundActionButtonBase.generated.h"

class UUICommonButtonBase;
class UBorder;
class USizeBox;
/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonBoundActionButtonBase : public UCommonBoundActionButton
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Button|Size")
	float DesiredIconSize = 64.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Button|Underline")
	FLinearColor UnderlineColorOff = FLinearColor(1.0f, 1.0f, 1.0f, 0.1f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Button|Underline")
	FLinearColor UnderlineColorOn = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

protected:
	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void OnWidgetRebuilt() override;

	virtual void NativeOnClicked() override;

	virtual void UpdateInputActionWidget() override;

private:
	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> ActionButton;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USizeBox> SizeBox;

	/** Widget Settings - Audio */
	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase",
			AllowPrivateAccess="true"))
	USoundBase* ClickSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float ClickVolume = 1.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadOnly,
		Category="UI Button|Interaction",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", UIMin="0.0", Units="s"))
	float ClickDelay = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(DisplayThumbnail = "true", AllowedClasses = "/Script/Engine.SoundBase",
			AllowPrivateAccess="true"))
	USoundBase* HoverSound = nullptr;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category="UI Button|Interaction",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float HoverVolume = 1.0f;

};
