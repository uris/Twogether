// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "UIOptionsListView.generated.h"

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

	UFUNCTION(BlueprintPure)
	bool IsScrollBarVisible() const;

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

private:
	UPROPERTY(EditAnywhere, Category="List View Settings")
	TObjectPtr<UData_OptionsListEntryMapping> DataLisEntryMapping;

	// Entries Generated Event Delegate
	FOnEntriesGenerated OnEntriesGeneratedEvent;

	// Entries Changed Event Delegate
	FOnEntriesChanged OnEntriesChangedEvent;

	// Entries Generated State
	int32 NumEntries = 0;

};
