#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"

class UGameUserSettings;

class TOGETHER_API FOptionsDataInteractionHelper
{
public:
	explicit FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath);

	FString GetValueAsString() const;

	void SetValueFromString(const FString& InStringValue) const;

private:
	FCachedPropertyPath CachedDynamicFunction;

	TWeakObjectPtr<UGameUserSettings> CachedWeakUserSettings;

};
