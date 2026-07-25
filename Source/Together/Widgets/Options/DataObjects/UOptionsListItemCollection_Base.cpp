// Uris - All Rights Reserved


#include "UOptionsListItemCollection_Base.h"

void UUOptionsListItemCollection_Base::AddChildListData(UOptionsListItemDataObject_Base* InChildData)
{

	// notify the child list data to emit itself
	InChildData->InitDataObject();

	// set the parent to this
	InChildData->SetParentData(this);

	ChildListDataArray.Add(InChildData);
}

TArray<UOptionsListItemDataObject_Base*> UUOptionsListItemCollection_Base::GetAllChildListData() const
{
	return ChildListDataArray;
}

bool UUOptionsListItemCollection_Base::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}
