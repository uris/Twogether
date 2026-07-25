// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListItemDataObject_Value.h"
#include "ListItemDataObject_String.generated.h"

USTRUCT(BlueprintType)
struct FStringSetting
{
	GENERATED_BODY()

	// text used to for displaying the setting value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SettingDataId = NAME_None;

	// text used to for displaying the setting value
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName = FText::GetEmpty();

	// value saved to the game settings file
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Value = TEXT("");

	FStringSetting() = default;

	FStringSetting(const FName& InSettingDataId, const FText& InDisplayName, const FString& InValue)
		: SettingDataId(InSettingDataId), DisplayName(InDisplayName), Value(InValue) {}
};

UENUM(BlueprintType)
enum class EStringSettingDirection : uint8
{
	Next UMETA(DisplayName = "Next"),
	Previous UMETA(DisplayName = "Previous")
};

/**
 *
 */
UCLASS()
class TOGETHER_API UListItemDataObject_String : public UListItemDataObject_Value
{
	GENERATED_BODY()

public:
	// overloaded add to settings array
	void AddDynamicSetting(const FName& InSettingDataId, const FText& InDisplayName, const FString& InValue);
	void AddDynamicSetting(const FStringSetting& InSetting);

	// enable setting next/previous settings as current
	FText CycleCurrentSetting(EStringSettingDirection InDirection = EStringSettingDirection::Next);

	// helper: get the display names text array only from the settings arrow for the rotator values
	TArray<FText> GetAvailableDisplayOptionsArray();

	// get the current display text
	FORCEINLINE const FText& GetCurrentDisplayText() const
	{
		return CurrentSetting.DisplayName;
	};

protected:
	// cache the currently displayed text and corresponding value
	FStringSetting CurrentSetting;
	TArray<FStringSetting> Settings;

	// override to initialize the settings array with the values needed
	virtual void OnDataObjectInitialized() override;

	virtual bool CanResetBackToDefault() const override;

	virtual bool ResetToDefault() override;

private:
	// helper: find entry in a settings array by value
	int32 GetSettingIndexByValue(const FString& InStringValue) const;

	// helper: check if the display name for a setting is not empty
	bool DidSetDisplayNameFromStringValue(const FString& InStringValue);
};
