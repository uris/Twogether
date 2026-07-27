// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UIOptionsListEntry.h"
#include "Widget_ListEntry_Scalar.generated.h"

class UUListItemDataObject_Scalar;
class UUISliderBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UWidget_ListEntry_Scalar : public UUIOptionsListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

	virtual void OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject) override;

	virtual void OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
	                                            EOptionsListModifiedReason InReason) override;

	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	virtual void NativeListEntryWidgetHovered(bool bInIsHovered) override;

	virtual void NativeListEntryWidgetSelected(bool bInIsSelected) override;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess="True"))
	TObjectPtr<UUISliderBase> Slider;

private:
	UPROPERTY(transient)
	UUListItemDataObject_Scalar* CachedOwningScalarObject;
};
