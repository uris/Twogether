// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Input/CommonBoundActionBar.h"
#include "UICommonBoundActionBarBase.generated.h"

/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonBoundActionBarBase : public UCommonBoundActionBar
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Properties")
	float Gap = 24.0f;

protected:
	virtual void OnWidgetRebuilt() override;
};