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

USTRUCT()
struct FResolvedEditCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FSettingEditCondition Condition;

	UPROPERTY(EditAnywhere)
	TWeakObjectPtr<UOptionsListItemDataObject_Base> TargetData;

	FResolvedEditCondition() = default;

	explicit FResolvedEditCondition(const FSettingEditCondition& InCondition,
	                                const TWeakObjectPtr<UOptionsListItemDataObject_Base> InTargetData)
		: Condition(InCondition), TargetData(InTargetData) {}
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
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, DescriptionImage)
	LIST_DATA_ACCESSOR(UOptionsListItemDataObject_Base*, ParentData)
	LIST_DATA_ACCESSOR(FSettingEditConditionDefinition, EditConditionDefinition)

	void InitDataObject();

	void SetShouldApplyChangesImmediately(bool InShouldApplyChangesImmediately);

	void SetApplyMode(EUserSettingApplyMode InApplyMode);

	void AddResolvedEditCondition(const FResolvedEditCondition& InEditCondition);

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
	void HandleEditConditionTargetModified(UOptionsListItemDataObject_Base* InModifiedData,
	                                       EOptionsListModifiedReason InReason);
	void RefreshEditability();

	void ClearDisabledText()
	{
		DisabledText = FText();
	}

	void InsertDisabledText(const FText& InDisabledText);

	// core info data
	FName DataId;
	FName UserDefinedDataId;
	FText DisplayName;
	FText Description;
	FText DisabledText;
	TSoftObjectPtr<UTexture2D> DescriptionImage;

	// behavior settings
	bool bShouldApplyChangesImmediately = false;
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;

	// edit conditions
	FSettingEditConditionDefinition EditConditionDefinition;
	bool bIsEditable = true;

	UPROPERTY(Transient)
	UOptionsListItemDataObject_Base* ParentData;

	UPROPERTY(transient)
	TArray<FResolvedEditCondition> ResolvedEditConditions;

};
