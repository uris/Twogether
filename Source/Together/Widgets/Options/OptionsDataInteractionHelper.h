#pragma once

#include "CoreMinimal.h"

class UUserSettings;

class TOGETHER_API FOptionsDataInteractionHelper
{
public:
	FOptionsDataInteractionHelper(FName InSettingId, const FString& InDefaultValue);

	FString GetValueAsString() const;

	void SetValueFromString(const FString& InStringValue) const;

private:
	TWeakObjectPtr<UUserSettings> CachedWeakUserSettings;

	FName SettingId;

	FString DefaultValue;
};
