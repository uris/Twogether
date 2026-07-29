// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UIOptionsListEntry.generated.h"

class UCommonTextStyle;
enum class EOptionsListModifiedReason : uint8;
class UUICommonTextBase;
class UOptionsListItemDataObject_Base;
class UCommonTextBlock;
/**
 *
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class TOGETHER_API UUIOptionsListEntry : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// create a method to request selection changes when pointer events are consumed by local widgets
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntrySelectionRequested, const UObject* /* ListItem */);

	// expose delegate so list view can consume this
	FOnEntrySelectionRequested OnEntrySelectionRequested;

	UPROPERTY(BlueprintReadOnly,
		Category = "UI Options List Entry|DisplayName",
		meta = (BindWidgetOptional))
	TObjectPtr<UUICommonTextBase> SettingDisplayName;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Options List Entry")
	void BP_NativeOnHovered(bool bHovered);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Options List Entry")
	void BP_NativeOnSelected(bool bSelected);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsHovered = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsSelected = false;

	// virtual interface for hover state
	virtual void NativeListEntryWidgetHovered(bool bInIsHovered);

	// virtual interface for selection state
	virtual void NativeListEntryWidgetSelected(bool bInIsSelected);

protected:
	// handle focus for gamepad input
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	// push the data to the item object
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// release resources
	virtual void NativeOnEntryReleased() override;

	// child class should overite this function to handle init needed, super is expected
	virtual void OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject);

	// child class calls this to update UI values after the data item has changed, super no needed
	virtual void OnOwningListDataObjectModified(UOptionsListItemDataObject_Base* InOwningListDataObject,
	                                            EOptionsListModifiedReason InReason);

	// receive edibility change notification for this entry and change state accordingly
	void HandleEditabilityChanged(bool bInIsEditable);

	// state override forwarded to virtual interface
	virtual void NativeOnItemSelectionChanged(bool bInIsSelected) override;

	// state override forwarded to virtual interface - child must call ::super:: to expose event to BP
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// state override forwarded to virtual interface - child must call ::super:: to expose event to BP
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	// expose a method for entry to request selection of this item to the owning list view
	void RequestOwningItemSelection() const;

	UPROPERTY(Transient)
	TObjectPtr<UObject> CachedListItemObject;
};
