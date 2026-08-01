// Uris - All Rights Reserved


#include "UWidget_ListEntry_Collection.h"

#include "ListEntryStyle.h"
#include "Components/Spacer.h"
#include "Widgets/Components/UICommonTextBase.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"


void UUWidget_ListEntry_Collection::OnOwningListDataObjectSet(UOptionsListItemDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningDataObject = InOwningListDataObject;
	ApplyStyles();
}

void UUWidget_ListEntry_Collection::NativePreConstruct()
{
	Super::NativePreConstruct();

	ApplyStyles();
}


void UUWidget_ListEntry_Collection::ApplyStyles()
{
	Super::ApplyStyles();

	if (TopSpacer && ListEntryStyle && IsValid(CachedOwningDataObject))
	{
		const bool bIsFirst = CachedOwningDataObject->GetbIsFirstEntry();
		TopSpacer->SetSize(
			{0.f, bIsFirst ? ListEntryStyle->TitleFirstEntryTopMargin : ListEntryStyle->TitleDefaultTopMargin});
	}

	if (SettingDisplayName && ListEntryStyle && ListEntryStyle->SectionTitleTextStyle.HoveredTextStyle)
	{
		SettingDisplayName->SetStyle(ListEntryStyle->SectionTitleTextStyle.HoveredTextStyle);
	}
}
