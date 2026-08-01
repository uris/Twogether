// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "OptionsListItemDataObject_Base.h"
#include "UOptionsListItemCollection_Base.generated.h"

/**
 *
 */
UCLASS()
class TOGETHER_API UUOptionsListItemCollection_Base : public UOptionsListItemDataObject_Base
{
	GENERATED_BODY()

public:
	void AddChildListData(UOptionsListItemDataObject_Base* InChildData);
	virtual TArray<UOptionsListItemDataObject_Base*> GetAllChildListData() const override;
	virtual bool HasAnyChildListData() const;
	int32 EntryIndex;

private:
	UPROPERTY(Transient)
	TArray<UOptionsListItemDataObject_Base*> ChildListDataArray;

};
