// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "Widgets/Options/DataObjects/ListItemDataObject_String.h"
#include "Widgets/Options/ListView/UIOptionsListEntry.h"
#include "Widget_ListEntry_String.generated.h"

class UVerticalBox;
class UListItemDataObject_String;
class UUICommonRotator;
class UUICommonTextBase;
class UUICommonButtonBase;
/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TOGETHER_API UWidget_ListEntry_String : public UUIOptionsListEntry
{
	GENERATED_BODY()

public:
	TObjectPtr<UListItemDataObject_String> GetOwningListDataObject() const
	{
		return CachedOwningListDataObject;
	};

	virtual void ApplyEditabilityToControls(const bool bInIsEditable) override;

protected:
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	virtual void NativeOnInitialized() override;

	virtual void NativePreConstruct() override;

	virtual void NativeDestruct() override;

	virtual void OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject) override;

	virtual void OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
	                                            EOptionsListModifiedReason InReason) override;

	virtual void NativeListEntryWidgetHovered(bool bInIsHovered) override;

	virtual void NativeListEntryWidgetSelected(bool bInIsSelected) override;

	virtual void ApplyStyles() override;

private:
	UPROPERTY(BlueprintReadOnly,
		Category = "UI Options List Entry",
		meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> CycleLeft;

	UPROPERTY(BlueprintReadOnly,
		Category = "UI Options List Entry",
		meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonButtonBase> CycleRight;

	UPROPERTY(BlueprintReadOnly,
		Category = "UI Options List Entry",
		meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonRotator> SettingRotator;

	UPROPERTY(Transient)
	TObjectPtr<UListItemDataObject_String> CachedOwningListDataObject;

	// cycle selected value
	void HandleCycleLeft() const;
	void HandleCycleRight() const;
	void HandleRotatorClicked() const;
	void HandleRotatedEvent(int32 Value, bool bUserInitiated) const;
	void CycleSelection(EStringSettingDirection InDirection) const;

	// utility to check is owner: unused for now
	bool IsListDataObjectOwner(UUIOptionsListEntry* InObject) const;
};
