// Uris - All Rights Reserved


#include "OptionsDataRegistry.h"

#include "OptionsDataInteractionHelper.h"
#include "UIFunctionLibrary.h"
#include "DataObjects/ListItemDataObject_String.h"
#include "DataObjects/UListItemDataObject_Scalar.h"
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
	SetupAudio(InitTabCollection("audio_tab_collection", "Audio"));
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
	// add setting to a tab collection
	TabCollection->AddChildListData(GameDifficulty);

	// TODO: Remove Test Item //
	UListItemDataObject_String* TestItem = NewObject<UListItemDataObject_String>();
	NewObject<UListItemDataObject_String>();

	TestItem->SetDataId(FName("TestItem"));
	TestItem->SetDisplayName(FText::FromString("Test Item"));
	TestItem->SetDescriptionImage(
		UUIFunctionLibrary::GetUISoftImageTextureByTag(UUIFunctionLibrary::GetGameplayTagFromString("UI.Image.Test")));
	TabCollection->AddChildListData(TestItem);
}

void UOptionsDataRegistry::SetupAudio(UUOptionsListItemCollection_Base* TabCollection)
{
	// set up volume category as a child collection object
	UUOptionsListItemCollection_Base* VolumeCollection = NewObject<UUOptionsListItemCollection_Base>();
	VolumeCollection->SetDataId(FName("VolumeCategoryCollection"));
	VolumeCollection->SetDisplayName(FText::FromString("Volume"));
	TabCollection->AddChildListData(VolumeCollection);

	// overall volume //
	{
		UUListItemDataObject_Scalar* OverallVolume = NewObject<UUListItemDataObject_Scalar>();
		OverallVolume->SetDataId(FName("OverallVolume"));
		OverallVolume->SetDisplayName(FText::FromString("Overall Volume"));
		OverallVolume->SetDescription(FText::FromString("Overall Volume Description"));
		OverallVolume->SetValueRange(TRange<float>(0.0f, 1.0f));
		OverallVolume->SetOutputRange(TRange<float>(0.0f, 2.0f));
		OverallVolume->SetSliderStepSize(0.01f);
		OverallVolume->SetDefaultValueFromString(LexToString(1.f));
		OverallVolume->SetValueType(ECommonNumericType::Percentage);
		OverallVolume->SetFormatting(UUListItemDataObject_Scalar::NoDecimal());
		// need getter and setter for this
		VolumeCollection->AddChildListData(OverallVolume);
	}

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

	UE_LOG(LogTemp, Warning, TEXT("Found Tab Collection: %d"), FoundTabCollection->HasAnyChildListData())

	// create local array to hold all children
	TArray<UOptionsListItemDataObject_Base*> AllChildListItems;

	// recurse to find all children and sub children
	FindChildListDataRecursive(FoundTabCollection, AllChildListItems);

	UE_LOG(LogTemp, Warning, TEXT("Total Children: %d"), AllChildListItems.Num());

	// return the complete array
	return AllChildListItems;
}

FString UOptionsDataRegistry::GetTabDisplayNameById(const FName& InSelectedTabId) const
{
	return GetListItemsBySelectedTabId(InSelectedTabId)[0]->GetDisplayName().ToString();
}

void UOptionsDataRegistry::FindChildListDataRecursive(const UUOptionsListItemCollection_Base* InParentCollection,
                                                      TArray<UOptionsListItemDataObject_Base*>& OutChildList) const
{
	if (!InParentCollection || !InParentCollection->HasAnyChildListData())
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent In Has No Children... Returning"));
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
