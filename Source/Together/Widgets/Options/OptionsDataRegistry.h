// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "DataObjects/ListItemDataObject_IntEnum.h"
#include "Settings/UserSettingTypes.h"
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

	// used for getting settings data id from native types or from user defined id
	static FName GetSettingIdString(const FUserSettingDefinition& Definition);

	// creates a base tab collection for each settings tab
	UUOptionsListItemCollection_Base* InitTabCollection(FName DataId, const FText& DisplayName);

	// creates a data object for each setting item
	UOptionsListItemDataObject_Base* CreateSettingDataObject(const FUserSettingDefinition& Definition);

	// get string values from user provided setting values
	static TArray<FStringSetting> GetStringSettings(const FName& SettingId,
	                                                const FUserSettingDefinition& Definition);

	// helper: create normalized string settings values for the base data object
	static TArray<FStringSetting> GetNativeStringSettings(const FUserSettingDefinition& Definition);

	// helper: create normalized scalar setting values for the base data object
	static FScalarSettingValues GetNativeScalarSettings(const FUserSettingDefinition& Definition);

	// helper: create resolved edit conditions after all items has been created for a tab group
	static void ProcessEditConditions(const TMap<FName, UOptionsListItemDataObject_Base*>& AllItemsById);

	// helper: create resolved setting dependencies after all items have been created for a tab group
	static void ProcessSettingDependencies(const TMap<FName, UOptionsListItemDataObject_Base*>& AllItemsById,
	                                       FName TabId);

	// helper: normalized setting type based on whether setting is native
	static EUserSettingValueType NormalizedSettingType(const FUserSettingDefinition& Definition);

	// helper: switch to call templated function for creating string settings values for native enum
	TArray<FStringSetting> GetNativeEnumSettingValues(const FUserSettingDefinition& Definition);

	// helper: get regular settings values from provided list of values
	static TArray<FStringSetting> GetEnumSettingValues(const FUserSettingDefinition& Definition);

	// helper: template to create string settings from a specified enum type
	template <typename EnumType>
	TArray<FStringSetting> EnumTypeToStringSettings(const FName& InDataId, const bool bAddCustom = false);
};
