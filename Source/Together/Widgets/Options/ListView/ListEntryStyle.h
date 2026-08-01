// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ListEntryStyle.generated.h"

class UCommonTextStyle;

USTRUCT(BlueprintType)
struct FListTextStyle
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	TSubclassOf<UCommonTextStyle> DefaultTextStyle = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	TSubclassOf<UCommonTextStyle> HoveredTextStyle = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	TSubclassOf<UCommonTextStyle> DisabledTextStyle = nullptr;

	FListTextStyle() = default;
};

/**
 *
 */
UCLASS(BlueprintType)
class TOGETHER_API UListEntryStyle : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	FListTextStyle SectionTitleTextStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	FListTextStyle ItemTextStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Text Styles")
	FListTextStyle ValueTextStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Padding")
	float DefaultPadding = 24.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Padding")
	float IndentPadding = 64.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Padding")
	float TitleDefaultTopMargin = 64.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Padding")
	float TitleFirstEntryTopMargin = 8.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Icons")
	float IconSize = 48.f;

};
