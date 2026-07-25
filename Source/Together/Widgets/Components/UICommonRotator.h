// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "UICommonRotator.generated.h"

class UUICommonButtonBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UUICommonRotator : public UCommonRotator
{
	GENERATED_BODY()

public:
	void SetSelectedOptionByText(const FText& InTextOption);

	void SetTextStyle(const TSubclassOf<UCommonTextStyle> InTextStyle);

private:
	int32 GetIndexByTextValue(const FText& InText) const;

};
