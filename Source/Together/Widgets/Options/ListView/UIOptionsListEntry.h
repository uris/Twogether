// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UIOptionsListEntry.generated.h"

class UBorder;
class USizeBox;
class UListEntryStyle;
class UVerticalBox;
class UCommonTextStyle;
enum class EOptionsListModifiedReason : uint8;
class UUICommonTextBase;
class UOptionsListItemDataObject_Base;
class UCommonTextBlock;

UENUM(BlueprintType)
enum class EStateChangeType : uint8
{
	Editable UMETA(DisplayName="Editable"),
	Hovered UMETA(DisplayName="Hovered"),
	Selected UMETA(DisplayName="Selected"),
};

USTRUCT(BlueprintType)
struct FListEntryState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEditable = true;

	bool IsActive() const
	{
		return bHovered || bSelected;
	}

	FListEntryState() = default;

	FListEntryState(const bool bInSelected, const bool bInHovered, const bool bInEditable) :
		bSelected(bInSelected), bHovered(bInHovered), bEditable(bInEditable) {};
};

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
		Category = "Custom Properties|DisplayName",
		meta = (BindWidgetOptional))
	TObjectPtr<UUICommonTextBase> SettingDisplayName;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UVerticalBox> EntryWrapper;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> EntryBorderBottom;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<USizeBox> EntryBorderTop;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Background;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Style")
	TObjectPtr<UListEntryStyle> ListEntryStyle;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Options List Entry")
	void BP_NativeOnHovered(bool bHovered);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Options List Entry")
	void BP_NativeOnSelected(bool bSelected);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsEditable = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsHovered = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsSelected = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Properties | Appearance")
	bool bHoverHighlights = true;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties | Appearance",
		meta = (EditCondition = "bHoverHighlights"))
	FLinearColor BackgroundOn = FLinearColor(1.f, 1.f, 1.f, 0.025f);

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Custom Properties | Appearance",
		meta = (EditCondition = "bHoverHighlights"))
	FLinearColor BackgroundOff = FLinearColor(1.f, 1.f, 1.f, 0.f);

	UFUNCTION(BlueprintCallable, Category = "UI Options List Entry")
	UOptionsListItemDataObject_Base* GetOwningDataObject() const;

	// called to remove a bottom border
	void SetBorderVisibility(const bool bBottomVisible, const bool bTopVisible) const;

	// virtual interface for hover state
	virtual void NativeListEntryWidgetHovered(bool bInIsHovered);

	// virtual interface for selection state
	virtual void NativeListEntryWidgetSelected(bool bInIsSelected);

	// child instance overrides, so they can apply disabled state to actual controls, keeping the entry selectable
	virtual void ApplyEditabilityToControls(bool bInIsEditable);

protected:
	// empty override to apply global styles and for children to handle specific style updates
	virtual void ApplyStyles();

	virtual void NativePreConstruct() override;

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

	virtual void NativeOnStateChange(EStateChangeType StateChangeType, bool bStateValue);

	// expose a method for entry to request selection of this item to the owning list view
	void RequestOwningItemSelection() const;

	UPROPERTY(Transient)
	TObjectPtr<UObject> CachedListItemObject;

private:
	void SetIndent(bool bHasParent) const;

	void HoverBackground() const;
};
