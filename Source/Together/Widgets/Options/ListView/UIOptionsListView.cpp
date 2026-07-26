// Uris - All Rights Reserved


#include "Widgets/Options/ListView/UIOptionsListView.h"

#include "UIOptionsListEntry.h"
#include "Editor/WidgetCompilerLog.h"
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
