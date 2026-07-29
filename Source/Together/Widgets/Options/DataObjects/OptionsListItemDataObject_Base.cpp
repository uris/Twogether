// Uris - All Rights Reserved


#include "OptionsListItemDataObject_Base.h"

#include "Settings/NativeSettingsHelper.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettings.h"

void UOptionsListItemDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UOptionsListItemDataObject_Base::OnDataObjectInitialized()
{
	// override on children
}

void UOptionsListItemDataObject_Base::SetShouldApplyChangesImmediately(const bool InShouldApplyChangesImmediately)
{
	bShouldApplyChangesImmediately = InShouldApplyChangesImmediately;
}

void UOptionsListItemDataObject_Base::SetApplyMode(const EUserSettingApplyMode InApplyMode)
{
	ApplyMode = InApplyMode;
}

void UOptionsListItemDataObject_Base::NotifyListDataModified(UOptionsListItemDataObject_Base* InModifiedData,
                                                             const EOptionsListModifiedReason InReason) const
{
	OnListDataModified.Broadcast(InModifiedData, InReason);
	if (bShouldApplyChangesImmediately)
	{
		if (UUserSettings* UserSettings = UUserSettings::Get())
		{
			switch (ApplyMode)
			{
				case EUserSettingApplyMode::ApplyAll:
					UNativeSettingsHelper::ApplyResolutionSettings(false);
					UserSettings->ApplyNonResolutionSettings();
					break;

				case EUserSettingApplyMode::ApplyResolutionSettings:
					UNativeSettingsHelper::ApplyResolutionSettings(false);
					break;

				case EUserSettingApplyMode::ApplyNonResolutionSettings:
					UserSettings->ApplyNonResolutionSettings();
					break;
			}
			UserSettings->SaveSettings();
		}
	}
}
