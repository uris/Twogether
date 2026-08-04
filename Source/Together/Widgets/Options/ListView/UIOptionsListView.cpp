// Uris - All Rights Reserved


#include "Widgets/Options/ListView/UIOptionsListView.h"

#include "CommonUISubsystemBase.h"
#include "UIFunctionLibrary.h"
#include "UIOptionsListEntry.h"
#include "Editor/WidgetCompilerLog.h"
#include "UI/UISoundFXs.h"
#include "Widgets/Options/DataObjects/Data_OptionsListEntryMapping.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"
#include "Widgets/Options/DataObjects/UOptionsListItemCollection_Base.h"

#if WITH_EDITOR
void UUIOptionsListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!DataLisEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("No valid data asset assigned")
			+ GetClass()->GetName()
			+ TEXT(" needs a valid data asset to function properly")
			));
	}
}
#endif

void UUIOptionsListView::OnItemsChanged(const TArray<UObject*>& AddedItems, const TArray<UObject*>& RemovedItems)
{
	Super::OnItemsChanged(AddedItems, RemovedItems);

	if (!AddedItems.IsEmpty())
	{
		OnEntriesChanged().Broadcast(GetNumItems());
	}
}

bool UUIOptionsListView::IsScrollBarVisible() const
{
	return MyListView.IsValid() && MyListView->IsScrollbarNeeded();
}

bool UUIOptionsListView::IsListItemHovered() const
{
	return HoveredDataObject.Get() != nullptr;
}

bool UUIOptionsListView::IsListItemSelected() const
{
	return SelectedDataObject.Get() != nullptr;
}

UOptionsListItemDataObject_Base* UUIOptionsListView::GetListItemSelected() const
{
	return SelectedDataObject.Get();
}

UOptionsListItemDataObject_Base* UUIOptionsListView::GetListItemHovered() const
{
	return HoveredDataObject.Get();
}

void UUIOptionsListView::NativeOnEntriesGenerated()
{
	Super::NativeOnEntriesGenerated();

	if (const int32 Entries = GetNumItems(); Entries > 0)
	{
		NumEntries = GetNumItems();
	}
	OnEntriesGeneratedEvent.Broadcast(NumEntries);
}

bool UUIOptionsListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	// collection types should not be selectable - return false if item is subclassed as collection
	return !FirstSelectedItem->IsA<UUOptionsListItemCollection_Base>();
}

UUserWidget& UUIOptionsListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                               const TSubclassOf<UUserWidget> DesiredEntryClass,
                                                               const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}

	const TSubclassOf<UUIOptionsListEntry> FoundWidgetClass = DataLisEntryMapping->FindEntryWidgetClassByDataObject(
		CastChecked<UOptionsListItemDataObject_Base>(Item));

	if (FoundWidgetClass)
	{
		return GenerateTypedEntry<UUIOptionsListEntry>(FoundWidgetClass, OwnerTable);
	}

	return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}

void UUIOptionsListView::OnSelectionChangedInternal(UObject* FirstSelectedItem)
{
	SelectedDataObject = Cast<UOptionsListItemDataObject_Base>(FirstSelectedItem);

	Super::OnSelectionChangedInternal(FirstSelectedItem);
}

void UUIOptionsListView::HandleListEntryHovered(UUserWidget& EntryWidget)
{

	HoveredDataObject = Cast<UUIOptionsListEntry>(&EntryWidget)->GetOwningDataObject();
	if (HoveredDataObject.Get())
	{
		EmitSFX(HoverSFXTagName);
	}

	Super::HandleListEntryHovered(EntryWidget);

}

void UUIOptionsListView::HandleListEntryUnhovered(UUserWidget& EntryWidget)
{
	Super::HandleListEntryUnhovered(EntryWidget);

	const UUIOptionsListEntry* Entry = Cast<UUIOptionsListEntry>(&EntryWidget);

	if (!Entry)
	{
		return;
	}

	UOptionsListItemDataObject_Base* DataObject = Entry->GetOwningDataObject();

	if (HoveredDataObject.Get() == DataObject)
	{
		HoveredDataObject = nullptr;
	}
}

void UUIOptionsListView::EmitSFX(const FGameplayTag SFXTagName) const
{
	if (SFXTagName.ToString().IsEmpty())
	{
		return;
	}
	UUIFunctionLibrary::PlaySoundFX(this, SFXTagName);
}
