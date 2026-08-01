// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UIOptionsListEntry.h"
#include "UWidget_ListEntry_Collection.generated.h"

class USpacer;
/**
 *
 */
UCLASS(Abstract, BlueprintType)
class TOGETHER_API UUWidget_ListEntry_Collection : public UUIOptionsListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Properties|Padding")
	float DefaultTopMargin = 64.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Properties|Padding")
	float FirstEntryTopMargin = 8.f;

protected:
	virtual void ApplyStyles() override;

	virtual void OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject) override;

	virtual void NativePreConstruct() override;

private:
	UPROPERTY(BlueprintReadOnly,
		Category = "UI Options List Entry",
		meta = (BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<USpacer> TopSpacer;

	UPROPERTY(Transient)
	TObjectPtr<UOptionsListItemDataObject_Base> CachedOwningDataObject;

};
