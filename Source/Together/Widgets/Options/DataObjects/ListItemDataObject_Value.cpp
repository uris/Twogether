// Uris - All Rights Reserved


#include "Widgets/Options/DataObjects/ListItemDataObject_Value.h"

#include "Widgets/Options/OptionsDataInteractionHelper.h"

void UListItemDataObject_Value::SetDataDynamicGetter(
	const TSharedPtr<FOptionsDataInteractionHelper>& InDataDynamicGetter)
{
	DataDynamicGetter = InDataDynamicGetter;
}

void UListItemDataObject_Value::SetDataDynamicSetter(
	const TSharedPtr<FOptionsDataInteractionHelper>& InDataDynamicSetter)
{
	DataDynamicSetter = InDataDynamicSetter;
}

FString UListItemDataObject_Value::GetCurrentValueAsString() const
{
	return DataDynamicGetter
		       ? DataDynamicGetter->GetValueAsString()
		       : GetDefaultValueAsString();
}
