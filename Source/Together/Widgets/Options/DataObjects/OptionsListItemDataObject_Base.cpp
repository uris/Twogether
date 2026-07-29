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

void UOptionsListItemDataObject_Base::AddResolvedEditCondition(const FResolvedEditCondition& InEditCondition)
{
	// set resolved edit conditions
	ResolvedEditConditions.Add(InEditCondition);

	// bind to the targets' data object if it's an edit condition and not bound yet
	if (UOptionsListItemDataObject_Base* TargetData = InEditCondition.TargetData.Get();
		TargetData && !TargetData->OnListDataModified.IsBoundToObject(this))
	{
		TargetData->OnListDataModified.AddUObject(
			this,
			&ThisClass::HandleEditConditionTargetModified);
	}

	// on bind, refresh editability to set/reset the state
	RefreshEditability();
}

bool UOptionsListItemDataObject_Base::AreEditConditionsMet() const
{
	// must have at least 1 condition
	if (EditConditionDefinition.Conditions.IsEmpty())
	{
		return true;
	}

	// must have at least 1 condition
	if (ResolvedEditConditions.IsEmpty())
	{
		return false;
	}

	// AND / OR definition
	const bool bMatchAll =
		EditConditionDefinition.MatchType == EEditConditionGroupOperator::MatchAll;

	// enforce AND across all definitions
	if (bMatchAll &&
	    ResolvedEditConditions.Num() != EditConditionDefinition.Conditions.Num())
	{
		return false;
	}

	// iterate each condition and evaluate editability
	for (const FResolvedEditCondition& ResolvedCondition : ResolvedEditConditions)
	{
		const UOptionsListItemDataObject_Base* TargetData = ResolvedCondition.TargetData.Get();
		if (!TargetData)
		{
			if (bMatchAll)
			{
				return false;
			}
			continue;
		}

		const FSettingEditCondition& Condition = ResolvedCondition.Condition;
		const FString CurrentValue = TargetData->GetCurrentValueAsString();
		bool bConditionMet = false;

		switch (Condition.LogicalOperator)
		{
			case EEditConditionOperator::Equals:
				bConditionMet = CurrentValue.Equals(
					Condition.ComparisonStringValue,
					ESearchCase::CaseSensitive);
				break;

			case EEditConditionOperator::NotEquals:
				bConditionMet = !CurrentValue.Equals(
					Condition.ComparisonStringValue,
					ESearchCase::CaseSensitive);
				break;

			case EEditConditionOperator::GreaterThan:
			case EEditConditionOperator::LessThan:
			{
				double CurrentNumber = 0.0;
				double ComparisonNumber = 0.0;
				if (LexTryParseString(CurrentNumber, *CurrentValue) &&
				    LexTryParseString(ComparisonNumber, *Condition.ComparisonStringValue))
				{
					bConditionMet =
						Condition.LogicalOperator == EEditConditionOperator::GreaterThan
							? CurrentNumber > ComparisonNumber
							: CurrentNumber < ComparisonNumber;
				}
				break;
			}

			case EEditConditionOperator::Contains:
			{
				TArray<FString> ComparisonValues;
				Condition.CommaSeparatedComparisonValues.ParseIntoArray(
					ComparisonValues,
					TEXT(","),
					true);

				for (FString& ComparisonValue : ComparisonValues)
				{
					ComparisonValue.TrimStartAndEndInline();
				}

				bConditionMet = ComparisonValues.Contains(CurrentValue);
				break;
			}
		}

		if (bMatchAll && !bConditionMet)
		{
			return false;
		}

		if (!bMatchAll && bConditionMet)
		{
			return true;
		}
	}

	return bMatchAll;
}

void UOptionsListItemDataObject_Base::HandleEditConditionTargetModified(
	UOptionsListItemDataObject_Base*,
	EOptionsListModifiedReason)
{
	RefreshEditability();
}

void UOptionsListItemDataObject_Base::RefreshEditability()
{
	// process edit conditions to check editaiblity
	const bool bNewIsEditable = AreEditConditionsMet();

	// if same nothing to do
	if (bIsEditable == bNewIsEditable)
	{
		return;
	}

	// otherwise update state and broadcast so that list can refresh state
	bIsEditable = bNewIsEditable;
	OnEditabilityChanged.Broadcast(bIsEditable);
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
