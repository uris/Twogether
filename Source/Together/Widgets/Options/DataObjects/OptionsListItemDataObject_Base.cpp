// Uris - All Rights Reserved


#include "OptionsListItemDataObject_Base.h"

#include "Settings/UserSettings.h"

void UOptionsListItemDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UOptionsListItemDataObject_Base::SetShouldApplyChangesImmediately(const bool InShouldApplyChangesImmediately)
{
	bShouldApplyChangesImmediately = InShouldApplyChangesImmediately;
}

void UOptionsListItemDataObject_Base::OnDataObjectInitialized() {}

void UOptionsListItemDataObject_Base::NotifyListDataModified(UOptionsListItemDataObject_Base* InModifiedData,
                                                             const EOptionsListModifiedReason InReason) const
{
	OnListDataModified.Broadcast(InModifiedData, InReason);
	if (bShouldApplyChangesImmediately)
	{
		// UUserSettings::Get()->ApplySettings(true);
		UUserSettings::Get()->ApplyNonResolutionSettings();
		UUserSettings::Get()->SaveSettings();
	}
}
