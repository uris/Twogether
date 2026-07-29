// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListItemDataObject_String.h"
#include "GameFramework/GameUserSettings.h"

#include "ListItemDataObject_IntEnum.generated.h"
/**
 *
 */
UCLASS()
class TOGETHER_API UListItemDataObject_IntEnum : public UListItemDataObject_String
{
	GENERATED_BODY()

public:
	// helper: get a string representing the name of an enum option
	template <typename EnumType>
	static FString GetEnumOptionNameString(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		return StaticEnumOption->GetNameStringByValue(InEnumOption);
	}

	// helper: get the enum value represented by the value's name string
	template <typename EnumType>
	static EnumType GetEnumOptionValueFromNameString(const FString& InNameString)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		return StaticEnumOption->GetValueByNameString(InNameString);
	}

	// create and add the settings for the requested enum type
	bool AddDynamicSetting(EIntEnumType InEnumType);

protected:
	virtual void OnDataObjectInitialized() override;

private:
	// create settings from specified enum defined in the data object
	bool CreateIntEnumSettings(EIntEnumType InEnumType, const FName& InSettingDataId);

	// create an array of settings for a specified enum type
	template <typename EnumType>
	TArray<FStringSetting> GenerateSettingsArray(const FName& InSettingDataId);

};
