// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UIOptionsListEntry.h"
#include "Widgets/Components/UISliderBase.h"
#include "Widget_ListEntry_Scalar.generated.h"

class UVerticalBox;
class UListItemDataObject_Scalar;
/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TOGETHER_API UWidget_ListEntry_Scalar : public UUIOptionsListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void HandleOnValueChanged(float InValue) const;

	UFUNCTION()
	void HandleOnMouseCaptureBegin();

	virtual void NativeListEntryWidgetHovered(bool bInIsHovered) override;

	virtual void NativeListEntryWidgetSelected(bool bInIsSelected) override;

	virtual void ApplyEditabilityToControls(bool bInIsEditable) override;

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativePreConstruct() override;

	virtual void OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject) override;

	virtual void OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
	                                            EOptionsListModifiedReason InReason) override;

	virtual void ApplyStyles() override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess="True"))
	TObjectPtr<UUISliderBase> Slider;

private:
	UPROPERTY(transient)
	UListItemDataObject_Scalar* CachedOwningScalarObject;

	bool bUpdatingFromDataObject = false;
};
