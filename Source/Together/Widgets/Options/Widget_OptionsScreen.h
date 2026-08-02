// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Settings/UserSettingTypes.h"
#include "SharedTypes/SharedTypes.h"
#include "Widgets/Base/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class USizeBox;
enum class EOptionsListModifiedReason : uint8;
class UOptionsListItemDataObject_Base;
class UWidget_OptionsDetails;
class UUIOptionsListView;
class UBorder;
class UUICommonTabListWidgetBase;
class UOptionsDataRegistry;
/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TOGETHER_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	// handle selected tab: ufunction because if delegate
	UFUNCTION()
	void HandleTabSelected(FName TagId);

	UFUNCTION()
	void HandleEntriesGenerated(int32 NumEntries) const;

protected:
	// setup interfaces
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	void GetLastListEntry() const;

	// setup on activation
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// register delegate handlers for requesting selection
	void HandleEntryGenerated(UUserWidget& EntryWidget);
	void HandleEntrySelectionRequested(const UObject* ListItem) const;
	void HandleEntryHoveredChange(UObject* InItem, bool bIsHovered) const;
	void SetSelectedItemDetails(UObject* InItem = nullptr) const;
	void HandleEntrySelectionChange(UObject* InItem) const;
	void HandleListDataModified(UOptionsListItemDataObject_Base* ModifiedData, EOptionsListModifiedReason Reason);

	// generate registry
	UOptionsDataRegistry* GetOrCreateOptionsRegistry();

	// layout helpers
	virtual void HandleScreenResize(const FVector2D& NewScreenSize, const FVector2D& PreviousScreenSize);

private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUIOptionsListView> OptionsListView;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USizeBox> ListViewBottomBorder;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UUICommonTabListWidgetBase> TabOptionsWidget;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UBorder> TabOptionsBackground;

	UPROPERTY(meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UWidget_OptionsDetails> SettingDetails;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Screen Options|Transition",
		meta = (AllowPrivateAccess = "true"))
	bool bDelayBackgroundOpacity = false;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Screen Options|Transition",
		meta = (AllowPrivateAccess = "true", EditCondition = "bDelayBackgroundOpacity", EditConditionHides))
	FTransition BackgroundOpacityTransition;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Screen Options|Transition",
		meta = (AllowPrivateAccess = "true", DisplayName = "Start Background Opacity", ClampMin = "0.0", ClampMax =
			"1.0", EditCondition = "bDelayBackgroundOpacity", EditConditionHides))
	float BackgroundOpacityStart = 0.0f;

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		Category = "Screen Options|Transition",
		meta = (AllowPrivateAccess = "true", DisplayName = "End Background Opacity", ClampMin = "0.0", ClampMax = "1.0"
		))
	float BackgroundOpacityEnd = 0.7f;

	UPROPERTY(EditDefaultsOnly,
		Category = "Options Screen",
		meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;

	UPROPERTY(Transient)
	TArray<UOptionsListItemDataObject_Base*> ResettableData;

	// pointer to the current registry if tab options
	UPROPERTY(Transient)
	UOptionsDataRegistry* OptionsRegistry;

	// default action handles
	FUIActionBindingHandle ResetActionHandler;
	FUIActionBindingHandle BackActionHandler;

	// reset action button handler
	void OnResetAction();

	// back action handler
	void OnBackAction();

	// Reset action handlers
	void PushConfirmReset();
	void HandleResetConfirmationAction(EConfirmationButtonType Result);
	FString TabSelectedDisplayName = FString();

	// misc list helpers
	int32 GetFirstSelectableItemIndexInList() const;

	// do on-screen animations and cache state
	void StartBackgroundOpacityTransition();
	float EvaluateBackgroundTransitionAlpha(float Alpha) const;
	bool bBackgroundOpacityTransitionActive = false;
	float BackgroundOpacityTransitionElapsed = 0.0f;

	// screen resize helper
	FVector2D CachedSize = FVector2D::ZeroVector;
};
