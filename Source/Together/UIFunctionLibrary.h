// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/UIIconSet.h"
#include "UI/UISoundFXs.h"
#include "UIFunctionLibrary.generated.h"

class UWidget_ActivatableBase;
/**
 *
 */
UCLASS()
class TOGETHER_API UUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public :
	UFUNCTION(BlueprintPure, Category="UI Functions Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetFrontEndSoftWidgetClassByTag(
		UPARAM(meta = (Categories = "UI.Widget"))
		FGameplayTag InWidgetTag);

	UFUNCTION(BlueprintPure, Category="UI Functions Library")
	static TSoftObjectPtr<UTexture2D> GetUISoftImageTextureByTag(
		UPARAM(meta = (Categories = "UI.Image"))
		FGameplayTag InImageTag);

	UFUNCTION(BlueprintPure, Category="UI Functions Library|Icons")
	static bool FindUIIconDefinition(
		UPARAM(meta=(Categories="UI.Icon"))
		FGameplayTag IconTag,
		FUIIconDefinition& OutIconDefinition);

	UFUNCTION(BlueprintPure, Category="UI Functions Library|SoundFX")
	static bool FindSoundFXDefinition(
		UPARAM(meta=(Categories="UI.SoundFX"))
		FGameplayTag SoundFXTag,
		FUISoundFXDefinition& OutSoundDefinition);

	UFUNCTION(BlueprintPure, Category="UI Functions Library|Gameplay Tags")
	static TArray<FGameplayTag> GetLeafChildTags(const FGameplayTag ParentTag);

	UFUNCTION(BlueprintCallable, Category="UI Functions Library|SoundFX")
	static void PlaySoundFX(const UObject* WorldContextObject,
	                        const FGameplayTag SoundFXTag,
	                        float DesiredSoundLevel = 1.0f);

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	static FGameplayTag GetGameplayTagFromString(const FString& InTagString);

	UFUNCTION(BlueprintPure, Category = "Conversion")
	static float StringToFloat(const FString& InString);

	UFUNCTION(BlueprintPure, Category = "Conversion")
	static FString FloatToString(float InValue);
};
