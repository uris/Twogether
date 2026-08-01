// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Settings/UserSettingTypes.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "OptionsListItemDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const {return PropertyName;} \
	void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName;}

class UOptionsListItemDataObject_Base;
class UTexture2D;

UENUM(BlueprintType)
enum class EOptionsListModifiedReason : uint8
{
	DirectlyModified,
	DependencyModified,
	ResetToDefault
};

/**
 *
 */
UCLASS(Abstract)
class TOGETHER_API UOptionsListItemDataObject_Base : public UObject
{
	GENERATED_BODY()

public:
	// broadcast data modified
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate,
	                                     UOptionsListItemDataObject_Base*,
	                                     EOptionsListModifiedReason)

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditabilityChangedDelegate, bool)

	FOnListDataModifiedDelegate OnListDataModified;
	FOnEditabilityChangedDelegate OnEditabilityChanged;

	// get/set macros
	LIST_DATA_ACCESSOR(FName, DataId)
	LIST_DATA_ACCESSOR(FName, UserDefinedDataId)
	LIST_DATA_ACCESSOR(FText, DisplayName)
	LIST_DATA_ACCESSOR(FText, Description)
	LIST_DATA_ACCESSOR(FText, DisabledText)
	LIST_DATA_ACCESSOR(bool, bDisableInEditorPreview)
	LIST_DATA_ACCESSOR(bool, bSkipZeroIndex)
	LIST_DATA_ACCESSOR(bool, bIsFirstEntry)
	LIST_DATA_ACCESSOR(bool, bIsChildEntry)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, DescriptionImage)
	LIST_DATA_ACCESSOR(FDynamicWidget, DescriptionWidget)
	LIST_DATA_ACCESSOR(UOptionsListItemDataObject_Base*, ParentData)
	LIST_DATA_ACCESSOR(FSettingEditConditionDefinition, EditConditionDefinition)
	LIST_DATA_ACCESSOR(TArray<FSettingDependency>, DependencyDefinitions)


	void InitDataObject();

	void SetShouldApplyChangesImmediately(bool InShouldApplyChangesImmediately);

	void SetApplyMode(EUserSettingApplyMode InApplyMode);

	void AddResolvedEditCondition(const FResolvedEditCondition& InEditCondition);

	void AddResolvedSettingDependency(const FResolvedSettingDependency& InDependency);

	bool AreEditConditionsMet();

	bool IsEditable() const
	{
		return bIsEditable;
	}

	virtual bool HasDefaultValue() const
	{
		return false;
	};

	virtual bool CanResetBackToDefault() const
	{
		return false;
	};

	virtual bool ResetToDefault()
	{
		return false;
	}

	virtual FString GetCurrentValueAsString() const
	{
		return FString();
	}

	virtual bool SetCurrentValueFromDependency(const FString& InValue)
	{
		return false;
	}

	virtual TArray<UOptionsListItemDataObject_Base*> GetAllChildListData() const
	{
		return TArray<UOptionsListItemDataObject_Base*>();
	}

	virtual TArray<UOptionsListItemDataObject_Base*> GetAllChildListData()
	{
		return TArray<UOptionsListItemDataObject_Base*>();
	}

protected:
	// child classes overrides this to handle initializing each list item
	virtual void OnDataObjectInitialized();

	// helper function children can use to broadcast value changes
	virtual void NotifyListDataModified(UOptionsListItemDataObject_Base* InModifiedData,
	                                    EOptionsListModifiedReason InReason =
		                                    EOptionsListModifiedReason::DirectlyModified) const;

private:
	// core trigger for handling effects that should trigger from edit condition logic
	void HandleEditConditionTargetModified(UOptionsListItemDataObject_Base* InModifiedData,
	                                       EOptionsListModifiedReason InReason);

	// core trigger for handling effects that should trigger from dependency logic
	void HandleDependencyTargetModified(UOptionsListItemDataObject_Base* InModifiedData,
	                                    EOptionsListModifiedReason InReason);

	// apply and save modifications
	void NotifyListDataModified(UOptionsListItemDataObject_Base* InModifiedData,
	                            EOptionsListModifiedReason InReason,
	                            bool bInShouldApplyChangesImmediately,
	                            EUserSettingApplyMode InApplyMode) const;

	// trigger evaluating edit conditions
	void RefreshEditability();

	// helper to clear value od disabled text entry
	void ClearDisabledText()
	{
		DisabledText = FText();
	}

	// helper to append lines to the disabled rich text
	void InsertDisabledText(const FText& InDisabledText);

	// core info data
	FName DataId;
	FName UserDefinedDataId;
	FText DisplayName;
	FText Description;
	FText DisabledText;
	TSoftObjectPtr<UTexture2D> DescriptionImage;
	FDynamicWidget DescriptionWidget;

	// behavior settings
	bool bShouldApplyChangesImmediately = false;
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;

	// edit conditions
	FSettingEditConditionDefinition EditConditionDefinition;
	bool bIsEditable = true;
	bool bDisableInEditorPreview = false;

	UPROPERTY(transient)
	TArray<FResolvedEditCondition> ResolvedEditConditions;

	// special conditions
	bool bSkipZeroIndex = false;
	bool bIsFirstEntry = false;
	bool bIsChildEntry = false;

	// dependencies
	TArray<FSettingDependency> DependencyDefinitions;

	UPROPERTY(Transient)
	TArray<FResolvedSettingDependency> ResolvedSettingDependencies;

	TSet<TWeakObjectPtr<UOptionsListItemDataObject_Base>> DependencyBoundTargets;

	// misc.
	UPROPERTY(Transient)
	UOptionsListItemDataObject_Base* ParentData;

};
