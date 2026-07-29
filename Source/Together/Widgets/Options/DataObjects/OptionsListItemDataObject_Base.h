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
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate,
	                                     UOptionsListItemDataObject_Base*,
	                                     EOptionsListModifiedReason)

	FOnListDataModifiedDelegate OnListDataModified;

	LIST_DATA_ACCESSOR(FName, DataId)
	LIST_DATA_ACCESSOR(FText, DisplayName)
	LIST_DATA_ACCESSOR(FText, Description)
	LIST_DATA_ACCESSOR(FText, DisabledText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, DescriptionImage)
	LIST_DATA_ACCESSOR(UOptionsListItemDataObject_Base*, ParentData)

	void InitDataObject();

	void SetShouldApplyChangesImmediately(bool InShouldApplyChangesImmediately);

	void SetApplyMode(EUserSettingApplyMode InApplyMode);

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
	FName DataId;
	FText DisplayName;
	FText Description;
	FText DisabledText;
	TSoftObjectPtr<UTexture2D> DescriptionImage;
	bool bShouldApplyChangesImmediately = false;
	EUserSettingApplyMode ApplyMode = EUserSettingApplyMode::ApplyNonResolutionSettings;

	UPROPERTY(Transient)
	UOptionsListItemDataObject_Base* ParentData;


};
