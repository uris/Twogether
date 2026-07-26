// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UOptionsListItemDataObject_Base;
class UUOptionsListItemCollection_Base;
class ULocalPlayer;
/**
 *
 */
UCLASS()
class TOGETHER_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:
	void InitRegistry(ULocalPlayer* InOwningLocalPlayer);

	UFUNCTION(BlueprintCallable)
	TArray<UOptionsListItemDataObject_Base*> GetListItemsBySelectedTabId(const FName& InSelectedTabId) const;

	UFUNCTION(BlueprintCallable)
	FString GetTabDisplayNameById(const FName& InSelectedTabId) const;

	UPROPERTY(Transient)
	TArray<UUOptionsListItemCollection_Base*> OptionTabCollections;

private:
	void FindChildListDataRecursive(const UUOptionsListItemCollection_Base* InParentCollection, TArray<UOptionsListItemDataObject_Base*>& OutChildList) const;
	UUOptionsListItemCollection_Base* InitTabCollection(const FString& DataId, const FString& DisplayName);
	static void SetupGameplay(UUOptionsListItemCollection_Base* TabCollection);
	static void SetupAudio(UUOptionsListItemCollection_Base* TabCollection);

};
