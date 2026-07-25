// Uris - All Rights Reserved


#include "OptionsDataRegistry.h"

#include "OptionsDataInteractionHelper.h"
#include "DataObjects/ListItemDataObject_String.h"
#include "DataObjects/UOptionsListItemCollection_Base.h"
#include "Settings/UserSettings.h"

/*
TSharedPtr<FOptionsDataInteractionHelper> ConstructionHelper = MakeShared<FOptionsDataInteractionHelper>(
GET_FUNCTION_NAME_STRING_CHECKED(UUserSettings, GetGameDifficulty));
*/
#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UUserSettings, SetterOrGetterFuncName))


void UOptionsDataRegistry::InitRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	SetupGameplay(InitTabCollection("gameplay_tab_collection", "Gameplay"));
	InitTabCollection("audio_tab_collection", "Audio");
	InitTabCollection("video_tab_collection", "Video");
	InitTabCollection("Input_tab_collection", "Input");
}

UUOptionsListItemCollection_Base* UOptionsDataRegistry::InitTabCollection(
	const FString& DataId,
	const FString& DisplayName)
{
	UUOptionsListItemCollection_Base* TabCollection = NewObject<UUOptionsListItemCollection_Base>();
	TabCollection->SetDataId(FName(DataId));
	TabCollection->SetDisplayName(FText::FromString(DisplayName));
	OptionTabCollections.Add(TabCollection);
	return TabCollection;
}

void UOptionsDataRegistry::SetupGameplay(UUOptionsListItemCollection_Base* TabCollection)
{
	// game difficulty
	UListItemDataObject_String* GameDifficulty = NewObject<UListItemDataObject_String>();
	NewObject<UListItemDataObject_String>();

	// set game difficulty meta and display
	const FName DataId = FName("GameDifficulty");
	GameDifficulty->SetDataId(DataId);
	GameDifficulty->SetDisplayName(FText::FromString("Game Difficulty"));

	// set value options
	GameDifficulty->AddDynamicSetting(DataId, FText::FromString("Easy"), TEXT("easy"));
	GameDifficulty->AddDynamicSetting(DataId, FText::FromString("Normal"), TEXT("normal"));
	GameDifficulty->AddDynamicSetting(DataId, FText::FromString("Hard"), TEXT("hard"));

	// set default
	GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));

	// assign getters/setters for saving to user settings
	GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGameDifficulty));
	GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGameDifficulty));

	// apply changes immediately
	GameDifficulty->SetShouldApplyChangesImmediately(true);
	// add setting to tab collection
	TabCollection->AddChildListData(GameDifficulty);

	// TODO: Remove Test Item //
	UListItemDataObject_String* TestItem = NewObject<UListItemDataObject_String>();
	NewObject<UListItemDataObject_String>();

	TestItem->SetDataId(FName("TestItem"));
	TestItem->SetDisplayName(FText::FromString("Test Item"));
	TabCollection->AddChildListData(TestItem);
}

TArray<UOptionsListItemDataObject_Base*> UOptionsDataRegistry::GetListItemsBySelectedTabId(
	const FName& InSelectedTabId) const
{
	UUOptionsListItemCollection_Base* const* FoundTabCollectionPtr = OptionTabCollections.FindByPredicate(
		[InSelectedTabId](const UUOptionsListItemCollection_Base* AvailableTabCollection)
		{
			return AvailableTabCollection->GetDataId() == InSelectedTabId;
		});

	checkf(FoundTabCollectionPtr, TEXT("Tab Collection not found: %s"), *InSelectedTabId.ToString());

	const UUOptionsListItemCollection_Base* FoundTabCollection = *FoundTabCollectionPtr;

	return FoundTabCollection->GetAllChildListData();
}

FString UOptionsDataRegistry::GetTabDisplayNameById(const FName& InSelectedTabId) const
{
	return GetListItemsBySelectedTabId(InSelectedTabId)[0]->GetDisplayName().ToString();
}
