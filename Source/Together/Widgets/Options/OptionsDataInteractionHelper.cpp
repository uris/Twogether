#include "OptionsDataInteractionHelper.h"

#include "Settings/UserSettings.h"

FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterFuncPath) :
	CachedDynamicFunction(InSetterOrGetterFuncPath)
{
	CachedWeakUserSettings = UUserSettings::Get();
}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	FString ReturnValue;
	PropertyPathHelpers::GetPropertyValueAsString(CachedWeakUserSettings.Get(), CachedDynamicFunction, ReturnValue);

	return ReturnValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue) const
{
	UE_LOG(LogTemp, Warning, TEXT("SetValueFromString %s"), *InStringValue);
	PropertyPathHelpers::SetPropertyValueFromString(CachedWeakUserSettings.Get(), CachedDynamicFunction, InStringValue);
}
