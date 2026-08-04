// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "UIOptionsListEntry.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"
#include "UIOptionsListView.generated.h"

class UUIOptionsListEntry;
class UOptionsListItemDataObject_Base;
class UData_OptionsListEntryMapping;
/**
 *
 */
UCLASS()
class TOGETHER_API UUIOptionsListView : public UCommonListView
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntriesGenerated, int32 /* NumEntries */);

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntriesChanged, int32 /* NumEntries */);

	// list regenerated items
	FOnEntriesGenerated& OnEntriesGenerated()
	{
		return OnEntriesGeneratedEvent;
	};

	// list received new entries
	FOnEntriesChanged& OnEntriesChanged()
	{
		return OnEntriesChangedEvent;
	};

	// helper: is a scroll bar displayed in the list view
	UFUNCTION(BlueprintPure)
	bool IsScrollBarVisible() const;

	// helper: is there a hovered item in the list view
	UFUNCTION(BlueprintPure)
	bool IsListItemHovered() const;

	// helper: is there a selected item in the list view
	UFUNCTION(BlueprintPure)
	bool IsListItemSelected() const;

	// helper: get data object of currently selected item
	UFUNCTION(BlueprintPure)
	UOptionsListItemDataObject_Base* GetListItemSelected() const;

	// helper: get data object of currently hovered item
	UFUNCTION(BlueprintPure)
	UOptionsListItemDataObject_Base* GetListItemHovered() const;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	virtual void OnItemsChanged(const TArray<UObject*>& AddedItems, const TArray<UObject*>& RemovedItems) override;

	virtual void NativeOnEntriesGenerated() override;

	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;

	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item,
	                                                   TSubclassOf<UUserWidget> DesiredEntryClass,
	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual void HandleListEntryHovered(UUserWidget& EntryWidget) override;

	virtual void HandleListEntryUnhovered(UUserWidget& EntryWidget) override;

	virtual void OnSelectionChangedInternal(UObject* FirstSelectedItem) override;

private:
	UPROPERTY(EditAnywhere, Category="List View Settings")
	TObjectPtr<UData_OptionsListEntryMapping> DataLisEntryMapping;

	// Entries Generated Event Delegate
	FOnEntriesGenerated OnEntriesGeneratedEvent;

	// Entries Changed Event Delegate
	FOnEntriesChanged OnEntriesChangedEvent;

	// Entries Generated State
	int32 NumEntries = 0;

	// cached hovered item
	UPROPERTY(Transient)
	TObjectPtr<UOptionsListItemDataObject_Base> HoveredDataObject = nullptr;

	// cached selected item
	UPROPERTY(Transient)
	TObjectPtr<UOptionsListItemDataObject_Base> SelectedDataObject = nullptr;
};
