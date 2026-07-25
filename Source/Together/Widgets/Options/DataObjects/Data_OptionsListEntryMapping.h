// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data_OptionsListEntryMapping.generated.h"

class UUIOptionsListEntry;
class UOptionsListItemDataObject_Base;
/**
 *
 */
UCLASS()
class TOGETHER_API UData_OptionsListEntryMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	TSubclassOf<UUIOptionsListEntry> FindEntryWidgetClassByDataObject(
		UOptionsListItemDataObject_Base* InDataObject) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UOptionsListItemDataObject_Base>, TSubclassOf<UUIOptionsListEntry>> DataObjectListEntryMap;


};
