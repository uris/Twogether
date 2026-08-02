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

	// expose delegate as reference - enable AddUObject binding
	FOnEntriesGenerated& OnEntriesGenerated()
	{
		return OnEntriesGeneratedEvent;
	};

	UFUNCTION(BlueprintPure)
	bool IsScrollBarVisible() const;

protected:
	virtual void NativeOnEntriesGenerated() override;

	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;

	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item,
	                                                   TSubclassOf<UUserWidget> DesiredEntryClass,
	                                                   const TSharedRef<STableViewBase>& OwnerTable) override;

private:
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif

	UPROPERTY(EditAnywhere, Category="List View Settings")
	TObjectPtr<UData_OptionsListEntryMapping> DataLisEntryMapping;

	// Entries Generated Event Delegate
	FOnEntriesGenerated OnEntriesGeneratedEvent;

	// Entries Generated State
	int32 NumEntries = 0;

};
