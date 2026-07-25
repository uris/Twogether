// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "UObject/Object.h"
#include "UISoundFXs.generated.h"

struct FGameplayTag;
class USoundBase;

USTRUCT(BlueprintType)
struct TOGETHER_API FUISoundFXDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound FX")
	TSoftObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound FX")
	float DesiredSoundLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FUI_SFX_Setting
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SFXTagName = FGameplayTag();

	UPROPERTY(EditAnywhere,
		BlueprintReadWrite,
		meta=(ClampMin="0.0", UIMin="0.0", UIMax="2.0"))
	float DesiredVolume = 1.0f;
};

USTRUCT(BlueprintType)
struct TOGETHER_API FCoreSFX
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound FX")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sound FX")
	float DesiredSoundLevel = 1.0f;
};

/**
 *
 */
UCLASS()
class TOGETHER_API UUISoundFXs : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SoundFX", meta=(Categories="UI.SoundFX"))
	TMap<FGameplayTag, FUISoundFXDefinition> SoundFX;

	const FUISoundFXDefinition* FindSoundDefinition(const FGameplayTag& SoundFXTag) const;
};
