// Uris - All Rights Reserved


#include "Widgets/Options/DataObjects/Data_OptionsListEntryMapping.h"

#include "OptionsListItemDataObject_Base.h"
#include "Widgets/Options/ListView/UIOptionsListEntry.h"

TSubclassOf<UUIOptionsListEntry> UData_OptionsListEntryMapping::FindEntryWidgetClassByDataObject(
	UOptionsListItemDataObject_Base* InDataObject) const
{
	// protect for nullptr
	if (!InDataObject)
	{
		return TSubclassOf<UUIOptionsListEntry>();
	}

	// init loop value to class of the in list entry item
	UClass* DataObjectClass = InDataObject->GetClass();

	// while the data object class is not null
	while (DataObjectClass)
	{

		// cast list entry to it's base class type = UOptionsListItemDataObject_Base
		TSubclassOf<UOptionsListItemDataObject_Base> ConvertedDataObjectClass = TSubclassOf<
			UOptionsListItemDataObject_Base>(
			DataObjectClass);

		// Check if this is present in the entry map and return
		if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
		{
			return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
		}

		// otherwise set DataObjectClass to its parent class and loop again
		DataObjectClass = DataObjectClass->GetSuperClass();

	}

	// if nothing found return empty class
	return TSubclassOf<UUIOptionsListEntry>();
}
