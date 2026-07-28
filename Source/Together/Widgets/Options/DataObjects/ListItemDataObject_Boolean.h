// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListItemDataObject_String.h"
#include "ListItemDataObject_Boolean.generated.h"

/**
 *
 */
UCLASS()
class TOGETHER_API UListItemDataObject_Boolean : public UListItemDataObject_String
{
	GENERATED_BODY()

public:
	void AddDynamicSetting(const FBoolSetting& InSetting);

protected:
	virtual void OnDataObjectInitialized() override;

	void EnsureBoolSettings();

	TArray<FStringSetting> CreateBoolSettings(const FBoolSetting& InSetting) const;
};
