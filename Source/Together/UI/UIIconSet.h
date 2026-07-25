// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UIIconSet.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct TOGETHER_API FUIIconDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Icon")
	TSoftObjectPtr<UTexture2D> Texture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Icon")
	FVector2D DesiredSize = FVector2D(64.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Icon")
	FLinearColor Tint = FLinearColor::White;
};

UCLASS(BlueprintType)
class TOGETHER_API UUIIconSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Icons", meta=(Categories="UI.Icon"))
	TMap<FGameplayTag, FUIIconDefinition> Icons;

	const FUIIconDefinition* FindIcon(const FGameplayTag& IconTag) const;
};
