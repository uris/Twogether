// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTextBlock.h"
#include "UICommonTextBase.generated.h"

class UUICommonButtonBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextDidChange, FText, UpdatedText, bool, bIsEmpty);

UENUM(BlueprintType)
enum class ETextType : uint8
{
	Title UMETA(DisplayName = "Title"),
	Paragraph UMETA(DisplayName = "Paragraph"),
	Button UMETA(DisplayName = "Button"),
	ButtonDescription UMETA(DisplayName = "Button Description"),
	HelpText UMETA(DisplayName = "Help Text"),
};

/**
 *
 */
UCLASS(Blueprintable)
class TOGETHER_API UUICommonTextBase : public UCommonTextBlock
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UI Text|Events")
	FOnTextDidChange OnTextDidChanged;

protected:
	// override to bind button base delegate
	virtual void SynchronizeProperties() override;

	// override text change to broadcast it
	virtual void OnTextChanged() override;

	void SetupButtonDescriptionText();

	void SetupHelpText();

	// handle updating the text from a button hover delegate
	UFUNCTION()
	void HandleButtonDescriptionUpdated(UUICommonButtonBase* Button, const FText& Description);

	// handle updating the text based on help / hints
	UFUNCTION()
	void HandleHelpTextUpdate(UObject* Object, const FText& HelpText);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI Text")
	ETextType TextType = ETextType::Paragraph;

};
