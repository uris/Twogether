// Uris - All Rights Reserved


#include "Widget_OptionsScreen.h"

#include "Components/Border.h"
#include "ICommonInputModule.h"
#include "OnlineSubsystemUtils.h"
#include "OptionsDataRegistry.h"
#include "Widget_OptionsDetails.h"
#include "Components/VerticalBoxSlot.h"
#include "DataObjects/UOptionsListItemCollection_Base.h"
#include "Input/CommonUIInputTypes.h"
#include "ListView/UIOptionsListEntry.h"
#include "ListView/UIOptionsListView.h"
#include "Settings/UserSettings.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Utility/Debug.h"
#include "Widgets/Components/UICommonListViewSessionsBase.h"
#include "Widgets/Components/UICommonTabListWidgetBase.h"
#include "Widgets/Screens/Widget_Confirmation.h"

void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!ResetAction.IsNull())
	{
		ResetActionHandler = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetAction)
				)
			);
	}

	if (!ICommonInputModule::GetSettings().GetDefaultBackAction().IsNull())
	{
		BackActionHandler = RegisterUIActionBinding(
			FBindUIActionArgs(
				ICommonInputModule::GetSettings().GetDefaultBackAction(),
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackAction)
				)
			);
	}

}

void UWidget_OptionsScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabOptionsWidget)
	{
		TabOptionsWidget->OnTabSelected.AddUniqueDynamic(this, &ThisClass::HandleTabSelected);
	}

	if (OptionsListView)
	{
		OptionsListView->OnEntryWidgetGenerated().RemoveAll(this);
		OptionsListView->OnItemIsHoveredChanged().RemoveAll(this);
		OptionsListView->OnItemSelectionChanged().RemoveAll(this);
		OptionsListView->OnEntriesGenerated().RemoveAll(this);
		OptionsListView->OnEntriesChanged().RemoveAll(this);

		OptionsListView->OnEntryWidgetGenerated().AddUObject(this, &ThisClass::HandleEntryGenerated);
		OptionsListView->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::HandleEntryHoveredChange);
		OptionsListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::HandleEntrySelectionChange);
		OptionsListView->OnEntriesGenerated().AddUObject(this, &ThisClass::HandleEntriesGenerated);
		OptionsListView->OnEntriesChanged().AddUObject(this, &ThisClass::HandleEntriesChanged);

	}
}

void UWidget_OptionsScreen::NativeDestruct()
{
	if (TabOptionsWidget)
	{
		TabOptionsWidget->OnTabSelected.RemoveDynamic(this, &ThisClass::HandleTabSelected);
	}

	if (OptionsListView)
	{
		OptionsListView->OnEntryWidgetGenerated().RemoveAll(this);
		OptionsListView->OnItemIsHoveredChanged().RemoveAll(this);
		OptionsListView->OnItemSelectionChanged().RemoveAll(this);
		OptionsListView->OnEntriesGenerated().RemoveAll(this);
		OptionsListView->OnEntriesChanged().RemoveAll(this);

		for (UUserWidget* EntryWidget : OptionsListView->GetDisplayedEntryWidgets())
		{
			if (UUIOptionsListEntry* Entry = Cast<UUIOptionsListEntry>(EntryWidget))
			{
				Entry->OnEntrySelectionRequested.RemoveAll(this);
			}
		}
	}

	Super::NativeDestruct();
}

UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	if (const UObject* SelectedObject = OptionsListView->GetSelectedItem())
	{
		if (UUserWidget* SelectedEntryWidget = OptionsListView->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedEntryWidget;
		}
	}

	return Super::NativeGetDesiredFocusTarget();
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	StartBackgroundOpacityTransition();

	// set up the option registry
	OptionsRegistry = GetOrCreateOptionsRegistry();

	for (const UUOptionsListItemCollection_Base* Collection : OptionsRegistry->OptionTabCollections)
	{
		if (!Collection)
		{
			continue;
		}
		if (TabOptionsWidget->GetTabButtonBaseByID(Collection->GetDataId()) != nullptr)
		{
			continue;
		}
		TabOptionsWidget->RequestRegisterTab(Collection->GetDataId(), Collection->GetDisplayName());
	}
}

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	// on deactivating update/commit updated user settings, applying non-video options
	// to avoid unneeded screen flicker and sizing issues
	// UUserSettings::Get()->ApplySettings(true);
	UUserSettings::Get()->ApplyNonResolutionSettings();
	UUserSettings::Get()->SaveSettings();
}

void UWidget_OptionsScreen::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D NewSize = MyGeometry.GetLocalSize();
	if (CachedSize.IsZero())
	{
		CachedSize = NewSize;
	}
	else if (!NewSize.Equals(CachedSize))
	{
		CachedSize = NewSize;
		HandleScreenResize(NewSize, CachedSize);
	}

	if (!bBackgroundOpacityTransitionActive || !TabOptionsBackground)
	{
		return;
	}

	BackgroundOpacityTransitionElapsed += InDeltaTime;
	const float Delay = FMath::Max(0.0f, BackgroundOpacityTransition.Delay);
	if (BackgroundOpacityTransitionElapsed < Delay)
	{
		return;
	}

	const float Duration = FMath::Max(0.0f, BackgroundOpacityTransition.Duration);
	if (Duration <= KINDA_SMALL_NUMBER)
	{
		TabOptionsBackground->SetRenderOpacity(FMath::Clamp(BackgroundOpacityEnd, 0.0f, 1.0f));
		bBackgroundOpacityTransitionActive = false;
		return;
	}

	const float Progress = FMath::Clamp(
		(BackgroundOpacityTransitionElapsed - Delay) / Duration,
		0.0f,
		1.0f);
	const float EasedAlpha = EvaluateBackgroundTransitionAlpha(Progress);

	TabOptionsBackground->SetRenderOpacity(FMath::Lerp(
		FMath::Clamp(BackgroundOpacityStart, 0.0f, 1.0f),
		FMath::Clamp(BackgroundOpacityEnd, 0.0f, 1.0f),
		EasedAlpha));

	if (Progress >= 1.0f)
	{
		TabOptionsBackground->SetRenderOpacity(FMath::Clamp(BackgroundOpacityEnd, 0.0f, 1.0f));
		bBackgroundOpacityTransitionActive = false;
	}
}

void UWidget_OptionsScreen::HandleEntryGenerated(UUserWidget& EntryWidget)
{
	if (UUIOptionsListEntry* Entry = Cast<UUIOptionsListEntry>(&EntryWidget))
	{
		// clear previous bindings
		Entry->OnEntrySelectionRequested.RemoveAll(this);

		// register binding
		Entry->OnEntrySelectionRequested.AddUObject(this, &ThisClass::HandleEntrySelectionRequested);
	}
}

void UWidget_OptionsScreen::HandleEntrySelectionRequested(const UObject* ListItem) const
{
	if (OptionsListView && ListItem)
	{
		OptionsListView->SetSelectedItem(ListItem);
	}
}

void UWidget_OptionsScreen::HandleEntryHoveredChange(UObject* InItem, const bool bIsHovered) const
{
	SetSelectedItemDetails(bIsHovered ? InItem : nullptr);
}

void UWidget_OptionsScreen::HandleEntrySelectionChange(UObject* InItem) const
{
	SetSelectedItemDetails();
}

void UWidget_OptionsScreen::HandleListDataModified(UOptionsListItemDataObject_Base* ModifiedData,
                                                   EOptionsListModifiedReason Reason)
{
	// protect for no values
	if (!ModifiedData)
	{
		return;
	}

	// if you can reset to default, bind the reset action handler
	if (ModifiedData->CanResetBackToDefault())
	{
		ResettableData.AddUnique(ModifiedData);
		if (!GetActionBindings().Contains(ResetActionHandler))
		{
			AddActionBinding(ResetActionHandler);
		}
	}

	// otherwise if there is a valid handler, remove it
	else
	{
		if (ResettableData.Contains(ModifiedData))
		{
			ResettableData.Remove(ModifiedData);
		}
	}

	// finally, regardless, if the resettable array is empty, unbind the reset action handler
	if (ResettableData.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandler);
	}
}

void UWidget_OptionsScreen::SetSelectedItemDetails(UObject* InItem) const
{
	// protect for objects needed
	if (!OptionsListView || !SettingDetails)
	{
		return;
	}

	UOptionsListItemDataObject_Base* DetailsDataObject = nullptr;

	// grab selected/hivered data object from in item
	if (InItem)
	{
		DetailsDataObject = Cast<UOptionsListItemDataObject_Base>(InItem);
	}
	else if (OptionsListView->IsListItemHovered())
	{
		DetailsDataObject = OptionsListView->GetListItemSelected();
	}
	else if (OptionsListView->IsListItemSelected())
	{
		DetailsDataObject = OptionsListView->GetListItemSelected();
	}

	// if object exists set display of details, otherwise clear
	if (DetailsDataObject)
	{
		const FString WidgetClassName = DetailsDataObject->GetClass()->GetName();
		SettingDetails->UpdateDetailsView(DetailsDataObject, WidgetClassName);
	}
	else
	{
		SettingDetails->ClearDetailsView();
	}

}

void UWidget_OptionsScreen::HandleTabSelected(const FName TagId)
{

	// clear the details panel content
	if (SettingDetails)
	{
		SettingDetails->ClearDetailsView();
	}

	// Get tab collection
	const TArray<UOptionsListItemDataObject_Base*> FoundListItems = GetOrCreateOptionsRegistry()->
		GetListItemsBySelectedTabId(TagId);

	// get the selected tab name
	TabSelectedDisplayName = FoundListItems.IsEmpty() ? FString() : FoundListItems[0]->GetDisplayName().ToString();

	// set list view to collapsed if there are no items
	OptionsListView->SetVisibility(FoundListItems.IsEmpty()
		                               ? ESlateVisibility::Collapsed
		                               : ESlateVisibility::Visible);

	// clear any existing selections
	OptionsListView->ClearSelection();

	// set the list items to the found items
	OptionsListView->SetListItems(FoundListItems);

	// do refresh request
	OptionsListView->RequestRefresh();

	// default to the first item on the list as the selected state
	// navigate to the first item on the list (could be a non selectable header)
	const int32 SelectedIndex = GetFirstSelectableItemIndexInList();
	if (FoundListItems.IsValidIndex(SelectedIndex))
	{
		const UObject* SelectedItem = FoundListItems[SelectedIndex];
		UObject* FirstItem = FoundListItems[0];
		OptionsListView->SetSelectedItem(SelectedItem);
		OptionsListView->RequestNavigateToItem(FirstItem);
	}

	// reset defaults
	ResettableData.Reset();

	// find items that can be reset to default and add then to the array
	for (UOptionsListItemDataObject_Base* ListItem : FoundListItems)
	{
		if (!ListItem)
		{
			continue;
		}

		if (!ListItem->OnListDataModified.IsBoundToObject(this))
		{
			ListItem->OnListDataModified.AddUObject(this, &ThisClass::HandleListDataModified);
		}

		if (ListItem->CanResetBackToDefault())
		{
			ResettableData.AddUnique(ListItem);
		}
	}

	// if no items to reset, remove action binding on the rest action button
	if (ResettableData.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandler);
	}
	else
	{
		if (!GetActionBindings().Contains(ResetActionHandler))
		{
			AddActionBinding(ResetActionHandler);
		}
	}

}

void UWidget_OptionsScreen::StartBackgroundOpacityTransition()
{
	bBackgroundOpacityTransitionActive = false;
	BackgroundOpacityTransitionElapsed = 0.0f;

	if (!TabOptionsBackground)
	{
		return;
	}

	const float EndOpacity = FMath::Clamp(BackgroundOpacityEnd, 0.0f, 1.0f);
	if (!bDelayBackgroundOpacity)
	{
		TabOptionsBackground->SetRenderOpacity(EndOpacity);
		return;
	}

	TabOptionsBackground->SetRenderOpacity(FMath::Clamp(BackgroundOpacityStart, 0.0f, 1.0f));

	if (BackgroundOpacityTransition.Delay <= 0.0f
	    && BackgroundOpacityTransition.Duration <= KINDA_SMALL_NUMBER)
	{
		TabOptionsBackground->SetRenderOpacity(EndOpacity);
		return;
	}

	bBackgroundOpacityTransitionActive = true;
}

float UWidget_OptionsScreen::EvaluateBackgroundTransitionAlpha(const float Alpha) const
{
	const float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);

	switch (BackgroundOpacityTransition.TransitionType)
	{
		case ECustomTransitionType::EaseInOut:
			return FMath::InterpEaseInOut(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::EaseIn:
			return FMath::InterpEaseIn(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::EaseOut:
			return FMath::InterpEaseOut(0.0f, 1.0f, ClampedAlpha, 2.0f);

		case ECustomTransitionType::Curve:
			return BackgroundOpacityTransition.Curve
				       ? FMath::Clamp(BackgroundOpacityTransition.Curve->GetFloatValue(ClampedAlpha), 0.0f, 1.0f)
				       : ClampedAlpha;

		case ECustomTransitionType::Linear:
		default:
			return ClampedAlpha;
	}
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateOptionsRegistry()
{
	if (!OptionsRegistry)
	{
		OptionsRegistry = NewObject<UOptionsDataRegistry>();
		OptionsRegistry->InitRegistry(GetOwningLocalPlayer());
	}
	return OptionsRegistry;
}

void UWidget_OptionsScreen::OnResetAction()
{
	if (ResettableData.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No resettable data found"));
		return;
	}
	PushConfirmReset();
}

void UWidget_OptionsScreen::OnBackAction()
{
	Debug::Print("Back Action");
	DeactivateWidget();
}

void UWidget_OptionsScreen::PushConfirmReset()
{
	if (!UI)
	{
		UE_LOG(LogTemp, Warning, TEXT("No UI Subsystem to process reset"));
		return;
	}

	FConfirmationButtonLabels Labels;
	Labels.OkLabel = NSLOCTEXT("MainMenu", "QuitConfirm", "Reset");
	Labels.CancelLabel = NSLOCTEXT("MainMenu", "QuitCancel", "Cancel");

	// use tab display name
	const FString ConfirmationText = FString::Printf(
		TEXT("%s: The options you changed will be reset to their default setting."),
		*TabSelectedDisplayName);

	UI->PushConfirmationScreenToStack(
		EConfirmationScreenType::OkCancel,
		FText::FromString("Reset to Defaults?"),
		FText::FromString(ConfirmationText),
		Labels,
		[this](const EConfirmationButtonType Result)
		{
			HandleResetConfirmationAction(Result);
		});
}

void UWidget_OptionsScreen::HandleResetConfirmationAction(const EConfirmationButtonType Result)
{
	if (Result != EConfirmationButtonType::Cancel)
	{
		// track reset progress
		const TArray<UOptionsListItemDataObject_Base*> DataToReset = ResettableData;
		TArray<UOptionsListItemDataObject_Base*> FailedToResetData;

		// iterate values to reset and call reset
		for (UOptionsListItemDataObject_Base* Data : DataToReset)
		{
			if (!IsValid(Data))
			{
				continue;
			}
			if (!Data->ResetToDefault())
			{
				FailedToResetData.Add(Data);
			}
		}

		// Keep only items that failed so the user can retry them.
		ResettableData = MoveTemp(FailedToResetData);

		if (ResettableData.IsEmpty())
		{
			RemoveActionBinding(ResetActionHandler);
			return;
		}

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Failed to reset %d option(s) to their defaults."),
			ResettableData.Num());
	}
}

int32 UWidget_OptionsScreen::GetFirstSelectableItemIndexInList() const
{
	if (!OptionsListView)
	{
		return INDEX_NONE;
	}

	for (int i = 0; i < OptionsListView->GetNumItems(); ++i)
	{
		// get list item
		UObject* Object = OptionsListView->GetItemAt(i);

		// collection items are not selectable
		if (!IsValid(Cast<UUOptionsListItemCollection_Base>(Object)))
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UWidget_OptionsScreen::HandleScreenResize(const FVector2D& NewScreenSize, const FVector2D& PreviousScreenSize)
{
	if (OptionsListView)
	{
		OptionsListView->RegenerateAllEntries();
	}
}

void UWidget_OptionsScreen::HandleEntriesGenerated(const int32 NumEntries) const
{
	if (!OptionsListView)
	{
		return;
	}

	const bool bScrolls = OptionsListView->IsScrollBarVisible();
	SetListSlotSize(bScrolls ? ESlateSizeRule::Fill : ESlateSizeRule::Automatic);
}

void UWidget_OptionsScreen::HandleEntriesChanged(const int32 NumEntries) const
{
	SetListSlotSize(ESlateSizeRule::Fill);
}

void UWidget_OptionsScreen::SetListSlotSize(const ESlateSizeRule::Type InSizeRule) const
{
	if (!OptionsListView)
	{
		return;
	}
	if (UVerticalBoxSlot* ListViewSlot = Cast<UVerticalBoxSlot>(OptionsListView->Slot))
	{
		if (ListViewSlot->GetSize().SizeRule == InSizeRule)
		{
			return;
		}

		FSlateChildSize ChildSize;
		ChildSize.SizeRule = InSizeRule;
		ChildSize.Value = 1.f;

		const EVerticalAlignment VAlign = InSizeRule == ESlateSizeRule::Fill ? VAlign_Fill : VAlign_Top;

		ListViewSlot->SetSize(ChildSize);
		ListViewSlot->SetHorizontalAlignment(HAlign_Fill);
		ListViewSlot->SetVerticalAlignment(VAlign);
	}
}
