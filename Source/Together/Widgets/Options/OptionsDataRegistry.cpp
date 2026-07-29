// Uris - All Rights Reserved


#include "OptionsDataRegistry.h"

#include "OptionsDataInteractionHelper.h"
#include "DataObjects/ListItemDataObject_Boolean.h"
#include "DataObjects/ListItemDataObject_IntEnum.h"
#include "DataObjects/ListItemDataObject_String.h"
#include "DataObjects/ListItemDataObject_Scalar.h"
#include "Settings/NativeSettingsHelper.h"
#include "DataObjects/UOptionsListItemCollection_Base.h"
#include "Engine/DataTable.h"
#include "Settings/TogetherSettings.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettingTypesNative.h"

namespace
{
struct FRuntimeSettingTab
{
	EUserSettingTab Tab;
	FName Id;
	FText DisplayName;
	int32 SortOrder;
	int32 EnumIndex;
};
}

void UOptionsDataRegistry::InitRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	OptionTabCollections.Reset();

	const UTogetherSettings* ProjectSettings = GetDefault<UTogetherSettings>();
	const UDataTable* SettingsTable = ProjectSettings
		                                  ? ProjectSettings->GameSettings.LoadSynchronous()
		                                  : nullptr;

	// protect for existing settings table
	if (!SettingsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to initialize options: no Game Settings data table is configured."));
		return;
	}

	// protect for correct structure of the settings table
	if (SettingsTable->GetRowStruct() != FUserSettingDefinition::StaticStruct())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Unable to initialize options: '%s' does not use FUserSettingDefinition."),
			*SettingsTable->GetName());
		return;
	}

	// create array to hold all the setting definitions extracted from the settings table
	TArray<const FUserSettingDefinition*> Definitions;

	// Load all rows of settings into Definitions array
	SettingsTable->GetAllRows(TEXT("UOptionsDataRegistry::InitRegistry"), Definitions);

	// create the tabs from the tabs enum
	const UEnum* TabEnum = StaticEnum<EUserSettingTab>();
	if (!TabEnum)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to initialize options: EUserSettingTab reflection data is unavailable."));
		return;
	}

	TMap<EUserSettingTab, const FUserSettingTabDefinition*> AuthoredTabs;
	if (ProjectSettings && !ProjectSettings->GameSettingTabs.IsNull())
	{
		if (const UDataTable* TabsTable = ProjectSettings->GameSettingTabs.LoadSynchronous())
		{
			if (TabsTable->GetRowStruct() == FUserSettingTabDefinition::StaticStruct())
			{
				TArray<const FUserSettingTabDefinition*> TabRows;
				TabsTable->GetAllRows(TEXT("UOptionsDataRegistry::InitRegistry"), TabRows);
				for (const FUserSettingTabDefinition* TabRow : TabRows)
				{
					if (TabRow)
					{
						AuthoredTabs.Add(TabRow->Tab, TabRow);
					}
				}
			}
			else
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Game Setting Tabs table '%s' does not use FUserSettingTabDefinition; using enum order."),
					*TabsTable->GetName());
			}
		}
	}

	TArray<FRuntimeSettingTab> RuntimeTabs;
	for (int32 EnumIndex = 0; EnumIndex < TabEnum->NumEnums(); ++EnumIndex)
	{
		const FString EnumName = TabEnum->GetNameStringByIndex(EnumIndex);
		bool bIsHidden = false;
#if WITH_METADATA
		bIsHidden = TabEnum->HasMetaData(TEXT("Hidden"), EnumIndex);
#endif
		if (bIsHidden || EnumName.EndsWith(TEXT("_MAX"), ESearchCase::IgnoreCase))
		{
			continue;
		}

		// get the int value of the current tab and ensure it is in range
		const int64 EnumValue = TabEnum->GetValueByIndex(EnumIndex);
		if (EnumValue == INDEX_NONE || EnumValue > MAX_uint8)
		{
			continue;
		}

		const EUserSettingTab CurrentTab = static_cast<EUserSettingTab>(EnumValue);
		const FName TabId(EnumName);
		const FUserSettingTabDefinition* const* AuthoredTab = AuthoredTabs.Find(CurrentTab);

		FRuntimeSettingTab& RuntimeTab = RuntimeTabs.AddDefaulted_GetRef();
		RuntimeTab.Tab = CurrentTab;
		RuntimeTab.Id = TabId;
		RuntimeTab.DisplayName =
			AuthoredTab && !(*AuthoredTab)->DisplayName.IsEmpty()
				? (*AuthoredTab)->DisplayName
				: TabEnum->GetDisplayNameTextByIndex(EnumIndex);
		RuntimeTab.SortOrder = AuthoredTab ? (*AuthoredTab)->SortOrder : EnumIndex;
		RuntimeTab.EnumIndex = EnumIndex;
	}

	RuntimeTabs.Sort(
		[](const FRuntimeSettingTab& Left, const FRuntimeSettingTab& Right)
		{
			return Left.SortOrder == Right.SortOrder
				       ? Left.EnumIndex < Right.EnumIndex
				       : Left.SortOrder < Right.SortOrder;
		});

	for (const FRuntimeSettingTab& RuntimeTab : RuntimeTabs)
	{
		const EUserSettingTab CurrentTab = RuntimeTab.Tab;
		const FName TabId = RuntimeTab.Id;
		UUOptionsListItemCollection_Base* TabCollection =
			InitTabCollection(TabId, RuntimeTab.DisplayName);

		TArray<const FUserSettingDefinition*> TabDefinitions;
		for (const FUserSettingDefinition* Definition : Definitions)
		{
			if (Definition && Definition->SettingTab == CurrentTab)
			{
				TabDefinitions.Add(Definition);
			}
		}

		TabDefinitions.Sort(
			[](const FUserSettingDefinition& Left, const FUserSettingDefinition& Right)
			{
				if (Left.ParentSettingId == Right.ParentSettingId &&
				    Left.SortOrder != Right.SortOrder)
				{
					return Left.SortOrder < Right.SortOrder;
				}
				return GetSettingIdString(Left).LexicalLess(GetSettingIdString(Right));
			});

		// create key/value map with each setting and it's base data object
		TMap<FName, UOptionsListItemDataObject_Base*> ItemsById;

		// iterate all settings definitions to create the uber list of settings for the tab
		for (const FUserSettingDefinition* Definition : TabDefinitions)
		{
			const FName EffectiveSettingId = GetSettingIdString(*Definition);

			// ignore all settings that don't have a settings id
			if (EffectiveSettingId.IsNone() || EffectiveSettingId == NativeSettingIds::Invalid)
			{
				UE_LOG(LogTemp,
				       Warning,
				       TEXT("Skipping a setting in tab '%s' because its effective SettingId is invalid."),
				       *TabId.ToString());
				continue;
			}

			// create a null base data object for the item
			UOptionsListItemDataObject_Base* Item = nullptr;

			// for items that are "collections" only (ie a setting group)
			// create a collection object for the item
			if (Definition->bIsSettingGroup)
			{
				Item = NewObject<UUOptionsListItemCollection_Base>(this);
				Item->SetDataId(EffectiveSettingId);
				Item->SetDisplayName(Definition->DisplayName);
				Item->SetDescription(Definition->Description);
				Item->SetDisabledText(Definition->DisabledText);
				Item->SetDescriptionImage(Definition->DescriptionImage);
			}

			// if the items are root settings, with actual values
			// create a full data object for the item
			else
			{
				Item = CreateSettingDataObject(*Definition);
			}

			// ensure an item was created
			if (!Item)
			{
				continue;
			}

			// if the created items appears to be a duplicate, silently warn and pass over
			if (ItemsById.Contains(EffectiveSettingId))
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Skipping duplicate setting id '%s' in tab '%s'."),
					*EffectiveSettingId.ToString(),
					*TabId.ToString());
				continue;
			}

			// add the item to the map of items
			ItemsById.Add(EffectiveSettingId, Item);
		}

		// iterate through all settings adding them to the correct parent tab
		for (const FUserSettingDefinition* Definition : TabDefinitions)
		{
			const FName EffectiveSettingId = GetSettingIdString(*Definition);

			// ensure an item data object exists for the matching setting id
			UOptionsListItemDataObject_Base* const* ItemPtr = ItemsById.Find(EffectiveSettingId);
			if (!ItemPtr)
			{
				continue;
			}

			// by default set the parent to the overall tab collection
			UUOptionsListItemCollection_Base* ParentCollection = TabCollection;

			// for settings that have a parent ID (belong to a group)
			if (!Definition->ParentSettingId.IsNone())
			{

				// create a temp parent pointer to the item's parent
				UOptionsListItemDataObject_Base* const* ParentPtr = ItemsById.Find(Definition->ParentSettingId);

				// rest the parent collection to the found items's parent or null if the parent was not found
				ParentCollection = ParentPtr ? Cast<UUOptionsListItemCollection_Base>(*ParentPtr) : nullptr;

				// if no parent was found for the item in the item collection, then we have a missing data issue.
				if (!ParentCollection)
				{
					UE_LOG(
						LogTemp,
						Warning,
						TEXT("Setting '%s' references missing or non-collection parent '%s'; attaching it to tab '%s'."
						),
						*Definition->SettingId.ToString(),
						*Definition->ParentSettingId.ToString(),
						*TabId.ToString());
					// repoint to the top level tab collection
					ParentCollection = TabCollection;
				}
			}

			// add the item as child
			ParentCollection->AddChildListData(*ItemPtr);
		}

		// once all items are created, do another pass to define edit dependencies
		ProcessEditConditions(ItemsById);

	}

}

// base settings tab collection creator
UUOptionsListItemCollection_Base* UOptionsDataRegistry::InitTabCollection(
	const FName DataId,
	const FText& DisplayName)
{
	UUOptionsListItemCollection_Base* TabCollection = NewObject<UUOptionsListItemCollection_Base>(this);
	TabCollection->SetDataId(DataId);
	TabCollection->SetDisplayName(DisplayName);
	OptionTabCollections.Add(TabCollection);
	return TabCollection;
}

// base data object item creatr
UOptionsListItemDataObject_Base* UOptionsDataRegistry::CreateSettingDataObject(
	const FUserSettingDefinition& Definition)
{

	// create each setting unique item data object properties based on value type
	UListItemDataObject_Value* ValueData = nullptr;
	switch (Definition.Type)
	{
		// string values
		case EUserSettingValueType::String:
		{
			UListItemDataObject_String* StringData =
				NewObject<UListItemDataObject_String>(this);
			StringData->SetDataId(GetSettingIdString(Definition));
			// get native settings from native helpers
			for (const FStringSetting& AvailableValue : Definition.bIsNativeSetting
				                                            ? GetNativeStringSettings(Definition)
				                                            : Definition.AvailableStringValues)
			{
				StringData->AddDynamicSetting(AvailableValue);
			}
			ValueData = StringData;
			break;
		}

		// string values
		case EUserSettingValueType::Bool:
		{
			UListItemDataObject_Boolean* BoolData =
				NewObject<UListItemDataObject_Boolean>(this);
			BoolData->SetDataId(GetSettingIdString(Definition));
			BoolData->AddDynamicSetting(Definition.AvailableBoolValues);
			ValueData = BoolData;
			break;
		}

		// enum values are shown as string
		case EUserSettingValueType::Integer:
		{
			UListItemDataObject_IntEnum* EnumData =
				NewObject<UListItemDataObject_IntEnum>(this);
			EnumData->SetDataId(GetSettingIdString(Definition));
			if (!EnumData->AddDynamicSetting(Definition.AvailableEnumValues))
			{
				return nullptr;
			}
			ValueData = EnumData;
			break;
		}

		// scalar values
		case EUserSettingValueType::Scalar:
		{
			UListItemDataObject_Scalar* ScalarData =
				NewObject<UListItemDataObject_Scalar>(this);
			const float MinValue = FMath::Min(Definition.MinValue, Definition.MaxValue);
			const float MaxValue = FMath::Max(Definition.MinValue, Definition.MaxValue);
			ScalarData->SetValueRange(TRange<float>(MinValue, MaxValue));
			ScalarData->SetOutputRange(TRange<float>(MinValue, MaxValue));
			ScalarData->SetSliderStepSize(FMath::Max(Definition.StepSize, UE_SMALL_NUMBER));
			ScalarData->SetValueType(Definition.NumericType);

			FCommonNumberFormattingOptions Formatting;
			Formatting.MinimumFractionalDigits =
				FMath::Max(0, Definition.MinimumFractionalDigits);
			Formatting.MaximumFractionalDigits =
				FMath::Max(
					Formatting.MinimumFractionalDigits,
					Definition.MaximumFractionalDigits);
			ScalarData->SetFormatting(Formatting);
			ValueData = ScalarData;
			break;
		}

		// log others for now, later we will create cases for them too
		default:
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Skipping setting '%s': value type '%s' does not have a list data object yet."),
				*Definition.SettingId.ToString(),
				*StaticEnum<EUserSettingValueType>()->GetNameStringByValue(
					static_cast<int64>(Definition.Type)));
			return nullptr;
	}

	// set the common data object properties
	ValueData->SetDataId(GetSettingIdString(Definition));
	ValueData->SetUserDefinedDataId(Definition.SettingId);
	ValueData->SetDisplayName(Definition.DisplayName);
	ValueData->SetDescription(Definition.Description);
	ValueData->SetDisabledText(Definition.DisabledText);
	ValueData->SetDescriptionImage(Definition.DescriptionImage);
	ValueData->SetDefaultValueFromString(Definition.DefaultValue);
	ValueData->SetShouldApplyChangesImmediately(Definition.bShouldApplyChangesImmediately);
	ValueData->SetApplyMode(Definition.ApplyMode);
	ValueData->SetEditConditionDefinition(Definition.EditConditions);

	// create default getters / setter for inserting and retrieving from user settings
	const TSharedPtr<FOptionsDataInteractionHelper> Interaction =
		MakeShared<FOptionsDataInteractionHelper>(
			GetSettingIdString(Definition),
			Definition.DefaultValue,
			Definition.bIsNativeSetting);
	ValueData->SetDataDynamicGetter(Interaction);
	ValueData->SetDataDynamicSetter(Interaction);

	// return the items value data
	return ValueData;
}

TArray<UOptionsListItemDataObject_Base*> UOptionsDataRegistry::GetListItemsBySelectedTabId(
	const FName& InSelectedTabId) const
{
	// find tab collection by predicate where tab id = parameter in
	UUOptionsListItemCollection_Base* const* FoundTabCollectionPtr = OptionTabCollections.FindByPredicate(
		[InSelectedTabId](const UUOptionsListItemCollection_Base* AvailableTabCollection)
		{
			return AvailableTabCollection->GetDataId() == InSelectedTabId;
		});

	// guard for found tab collection by ID
	checkf(FoundTabCollectionPtr, TEXT("Tab Collection not found: %s"), *InSelectedTabId.ToString());

	// deref the found collection
	const UUOptionsListItemCollection_Base* FoundTabCollection = *FoundTabCollectionPtr;

	// create local array to hold all children
	TArray<UOptionsListItemDataObject_Base*> AllChildListItems;

	// recurse to find all children and sub children
	FindChildListDataRecursive(FoundTabCollection, AllChildListItems);

	// return the complete array
	return AllChildListItems;
}

FString UOptionsDataRegistry::GetTabDisplayNameById(const FName& InSelectedTabId) const
{
	const UUOptionsListItemCollection_Base* const* FoundTabCollection =
		OptionTabCollections.FindByPredicate(
			[InSelectedTabId](const UUOptionsListItemCollection_Base* Collection)
			{
				return Collection && Collection->GetDataId() == InSelectedTabId;
			});

	return FoundTabCollection
		       ? (*FoundTabCollection)->GetDisplayName().ToString()
		       : FString();
}

void UOptionsDataRegistry::FindChildListDataRecursive(const UUOptionsListItemCollection_Base* InParentCollection,
                                                      TArray<UOptionsListItemDataObject_Base*>& OutChildList)
{
	if (!InParentCollection || !InParentCollection->HasAnyChildListData())
	{
		return;
	}

	for (UOptionsListItemDataObject_Base* ChildListData : InParentCollection->GetAllChildListData())
	{
		// go next if no child items
		if (!IsValid(ChildListData))
		{
			UE_LOG(LogTemp, Warning, TEXT("Child List Data Not Valid: Continuing"));
			continue;
		}

		// add child item to out collection
		OutChildList.Add(ChildListData);

		// recurse if more children available
		const UUOptionsListItemCollection_Base* ChildCollection = Cast<UUOptionsListItemCollection_Base>(ChildListData);
		if (ChildCollection && ChildCollection->HasAnyChildListData())
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Sub Collection: Recursing"));
			FindChildListDataRecursive(ChildCollection, OutChildList);
		}
	}

}

FName UOptionsDataRegistry::GetSettingIdString(const FUserSettingDefinition& Definition)
{
	if (!Definition.bIsNativeSetting)
	{
		return Definition.SettingId;
	}

	if (StaticEnum<ENativeUnrealSettings>())
	{
		return GetNativeSettingId(Definition.NativeSetting);
	}

	return FName();
}

TArray<FStringSetting> UOptionsDataRegistry::GetNativeStringSettings(const FUserSettingDefinition& Definition)
{
	switch (Definition.NativeSetting)
	{
		case ENativeUnrealSettings::ScreenResolution:
			return UNativeSettingsHelper::GetSupportedResolutionsSettings(
				GetNativeSettingId(Definition.NativeSetting));
		default:
			return {};
	}
}

void UOptionsDataRegistry::ProcessEditConditions(const TMap<FName, UOptionsListItemDataObject_Base*>& AllItemsById)
{
	// protect for valid data objects
	if (AllItemsById.IsEmpty())
	{
		return;
	}

	// flatten map to array
	TArray<UOptionsListItemDataObject_Base*> AllItems;
	AllItemsById.GenerateValueArray(AllItems);

	// find dependencies for the owning item in the array
	for (UOptionsListItemDataObject_Base* Item : AllItems)
	{
		// protect for existing setting
		if (!Item)
		{
			continue;
		}

		// add the dependency
		for (const FSettingEditCondition& Condition : Item->GetEditConditionDefinition().Conditions)
		{
			// match using the user supplied data id from the data row entry, not the resolved id
			// since native settings internally use their own setting id based on unreals nomenclature
			UOptionsListItemDataObject_Base* const* FoundItem = AllItems.FindByPredicate(
				[&Condition](const UOptionsListItemDataObject_Base* Setting)
				{
					return Setting &&
					       Setting->GetUserDefinedDataId() == Condition.TargetSettingDataId;
				});

			// set the target item
			UOptionsListItemDataObject_Base* TargetItem = FoundItem ? *FoundItem : nullptr;

			if (!TargetItem)
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Edit condition for '%s' references missing setting '%s'."),
					*Item->GetDataId().ToString(),
					*Condition.TargetSettingDataId.ToString());
				continue;
			}

			Item->AddResolvedEditCondition(FResolvedEditCondition(Condition, TargetItem));
		}

	}
}
