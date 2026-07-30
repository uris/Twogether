// Uris - All Rights Reserved

#include "OptionsListItemDataObject_Base.h"

#include "UIFunctionLibrary.h"
#include "Settings/NativeSettingsHelper.h"
#include "Settings/UserSettingTypes.h"
#include "Settings/UserSettings.h"

void UOptionsListItemDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UOptionsListItemDataObject_Base::OnDataObjectInitialized()
{
	RefreshEditability();
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

bool UOptionsListItemDataObject_Base::AreEditConditionsMet()
{
	// clean disabled text
	ClearDisabledText();

	// if the edit condition prevents modification in the editor
	UE_LOG(LogTemp,
	       Verbose,
	       TEXT("Preview Disabled: %d, In Editor: %d"),
	       bDisableInEditorPreview,
	       UUIFunctionLibrary::IsPreviewingInEditor());
	if (bDisableInEditorPreview && UUIFunctionLibrary::IsPreviewingInEditor())
	{
		InsertDisabledText(
			FText::FromString(TEXT("Cannot edit or modify this setting while in editor or and editor preview window")));
		return false;
	}

	// no conditions means it evaluates to match ok
	if (EditConditionDefinition.Conditions.IsEmpty())
	{
		return true;
	}

	// must have at least 1 condition, if error don't disable
	if (ResolvedEditConditions.IsEmpty())
	{
		InsertDisabledText(
			FText::FromString(TEXT("Unable to find edit conditions that should exist. Check setting definitions.")));
		return true;
	}

	// AND / OR definition
	const bool bMatchAll =
		EditConditionDefinition.MatchType == EEditConditionGroupOperator::MatchAll;

	// store edit conditions messages for each condition that evaluates to false
	TArray<FText> UnmetConditionMessages;

	// enforce AND across all definitions - on error resolve true
	if (bMatchAll &&
	    ResolvedEditConditions.Num() != EditConditionDefinition.Conditions.Num())
	{
		InsertDisabledText(
			FText::FromString(TEXT("Mismatched edit conditions. Check setting definitions.")));
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
				InsertDisabledText(
					FText::FromString(TEXT("Target setting not found. Check setting definitions.")));
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
			InsertDisabledText(FText::FromString(Condition.EditingDisabledMessage));
			return false;
		}

		if (!bMatchAll && bConditionMet)
		{
			return true;
		}

		if (!bMatchAll && !Condition.EditingDisabledMessage.IsEmpty())
		{
			UnmetConditionMessages.Add(FText::FromString(Condition.EditingDisabledMessage));
		}
	}

	// MatchAny only fails after every condition has failed. Preserve the messages
	// from those failed conditions so the details view can explain why editing is disabled.
	for (const FText& UnmetConditionMessage : UnmetConditionMessages)
	{
		InsertDisabledText(UnmetConditionMessage);
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
	// process edit conditions to check edibility
	const bool bNewIsEditable = AreEditConditionsMet();

	// if same nothing to do
	if (bIsEditable == bNewIsEditable)
	{
		return;
	}

	// otherwise update state
	bIsEditable = bNewIsEditable;

	// update the disabled text based on state
	SetDisabledText(bIsEditable ? FText::FromString(TEXT("")) : GetDisabledText());

	// broadcast the new state
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

void UOptionsListItemDataObject_Base::InsertDisabledText(const FText& InDisabledText)
{

	const FText Separator = DisabledText.IsEmpty() ? FText() : FText::FromString(TEXT("\n\n"));
	const FText TextToAdd = FText::Format(FText::FromString(TEXT("{0}{1}")), Separator, InDisabledText);
	DisabledText = FText::Format(FText::FromString(TEXT("{0}{1}")), DisabledText, TextToAdd);

}
