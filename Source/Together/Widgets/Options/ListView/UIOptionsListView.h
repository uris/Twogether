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
	UFUNCTION(BlueprintPure)
	bool IsScrollBarVisible() const;

protected:
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

};
