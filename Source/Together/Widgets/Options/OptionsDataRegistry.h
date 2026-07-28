// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UOptionsListItemDataObject_Base;
class UUOptionsListItemCollection_Base;
class ULocalPlayer;
struct FUserSettingDefinition;
/**
 *
 */
UCLASS()
class TOGETHER_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:
	// kicks of creating the settings tab collectins and the respective settings
	void InitRegistry(ULocalPlayer* InOwningLocalPlayer);

	UFUNCTION(BlueprintCallable)
	TArray<UOptionsListItemDataObject_Base*> GetListItemsBySelectedTabId(const FName& InSelectedTabId) const;

	UFUNCTION(BlueprintCallable)
	FString GetTabDisplayNameById(const FName& InSelectedTabId) const;

	UPROPERTY(Transient)
	TArray<UUOptionsListItemCollection_Base*> OptionTabCollections;

private:
	// gets all children of a specific collection
	static void FindChildListDataRecursive(const UUOptionsListItemCollection_Base* InParentCollection,
	                                       TArray<UOptionsListItemDataObject_Base*>& OutChildList);

	// creates a base tab collection for each settings tab
	UUOptionsListItemCollection_Base* InitTabCollection(FName DataId, const FText& DisplayName);

	// creates a data object for each setting item
	UOptionsListItemDataObject_Base* CreateSettingDataObject(const FUserSettingDefinition& Definition);

};
