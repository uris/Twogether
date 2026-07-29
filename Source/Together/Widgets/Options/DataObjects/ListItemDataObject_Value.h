// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/OptionsListItemDataObject_Base.h"
#include "ListItemDataObject_Value.generated.h"

class FOptionsDataInteractionHelper;
/**
 *
 */
UCLASS(Abstract)
class TOGETHER_API UListItemDataObject_Value : public UOptionsListItemDataObject_Base
{
	GENERATED_BODY()

public:
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDataDynamicGetter);
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDataDynamicSetter);

	void SetDefaultValueFromString(const FString& InDefaultStringValue)
	{
		DefaultStringValue = InDefaultStringValue;
	}

	FString GetDefaultValueAsString() const
	{
		return DefaultStringValue.IsSet() ? DefaultStringValue.GetValue() : "";
	}

	virtual FString GetCurrentValueAsString() const override;

	virtual bool HasDefaultValue() const override
	{
		return DefaultStringValue.IsSet();
	}

protected:
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

private:
	TOptional<FString> DefaultStringValue;
};
