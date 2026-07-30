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

protected:
	virtual void OnDataObjectInitialized() override;

};
