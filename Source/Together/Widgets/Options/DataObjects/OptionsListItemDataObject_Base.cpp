// Uris - All Rights Reserved

#include "OptionsListItemDataObject_Base.h"

#include "UIFunctionLibrary.h"
#include "Settings/UserSettings/NativeSettings/NativeSettingsHelper.h"
#include "Settings/UserSettings/Utils/SettingsDataUtility.h"
#include "Settings/UserSettings/UserSettingTypes/UserSettingTypes.h"
#include "Settings/UserSettings/UserSettings.h"

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

void UOptionsListItemDataObject_Base::AddResolvedSettingDependency(
	const FResolvedSettingDependency& InDependency)
{
	ResolvedSettingDependencies.Add(InDependency);

	if (UOptionsListItemDataObject_Base* DependantData = InDependency.DependantData.Get();
		DependantData && !DependencyBoundTargets.Contains(DependantData))
	{
		DependantData->OnListDataModified.AddUObject(
			this,
			&ThisClass::HandleDependencyTargetModified);
		DependencyBoundTargets.Add(DependantData);
	}
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
			FText::FromString(
				TEXT("Note: You cannot edit or modify this setting while in editor or and editor preview window")));
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
		const bool bConditionMet = FSettingsDataUtility::DoesValueMatch(
			Condition.LogicalOperator,
			CurrentValue,
			Condition.ComparisonStringValue,
			Condition.CommaSeparatedComparisonValues);

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

void UOptionsListItemDataObject_Base::HandleDependencyTargetModified(
	UOptionsListItemDataObject_Base* InModifiedData,
	const EOptionsListModifiedReason InReason)
{
	// Dependencies react only to explicit user edits. Initialization, reset, and
	// dependency-driven synchronization must not ripple into other settings.
	if (!InModifiedData || InReason != EOptionsListModifiedReason::DirectlyModified)
	{
		return;
	}

	// process each matched dependency
	for (const FResolvedSettingDependency& ResolvedDependency : ResolvedSettingDependencies)
	{
		if (ResolvedDependency.DependantData.Get() != InModifiedData)
		{
			continue;
		}

		const FSettingDependency& Dependency = ResolvedDependency.Dependency;
		const FString CurrentValue = InModifiedData->GetCurrentValueAsString();
		if (!FSettingsDataUtility::DoesValueMatch(
			    Dependency.LogicalOperator,
			    CurrentValue,
			    Dependency.ComparisonStringValue,
			    Dependency.CommaSeparatedComparisonValues))
		{
			continue;
		}

		FString ResultValue;

		switch (Dependency.DependencyResult)
		{
			case EDependencyResult::SetToValue:
				ResultValue = Dependency.Value;
				break;

			case EDependencyResult::SetToMatchThis:
				ResultValue = CurrentValue;
				break;

			case EDependencyResult::SetToMatchOther:
			{
				const UOptionsListItemDataObject_Base* OtherData =
					ResolvedDependency.OtherData.Get();
				if (!OtherData)
				{
					continue;
				}
				ResultValue = OtherData->GetCurrentValueAsString();
				break;
			}
		}

		if (SetCurrentValueFromDependency(ResultValue))
		{
			NotifyListDataModified(
				this,
				EOptionsListModifiedReason::DependencyModified,
				Dependency.bShouldApplyChangesImmediately,
				Dependency.ApplyMode);
		}
	}
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
	NotifyListDataModified(
		InModifiedData,
		InReason,
		bShouldApplyChangesImmediately,
		ApplyMode);
}

void UOptionsListItemDataObject_Base::NotifyListDataModified(
	UOptionsListItemDataObject_Base* InModifiedData,
	const EOptionsListModifiedReason InReason,
	const bool bInShouldApplyChangesImmediately,
	const EUserSettingApplyMode InApplyMode) const
{
	OnListDataModified.Broadcast(InModifiedData, InReason);
	if (bInShouldApplyChangesImmediately)
	{
		if (UUserSettings* UserSettings = UUserSettings::Get())
		{
			switch (InApplyMode)
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
