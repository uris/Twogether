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
	static int32 StringToInt(const FString& InString);

	UFUNCTION(BlueprintPure, Category = "Conversion")
	static FString FloatToString(float InValue);

	UFUNCTION(BlueprintPure, Category = "Editor Preview")
	static bool IsPreviewingInEditor();

	// transform an enum entry into float values along a provided range, mapping the
	// total number of entries to the range and deriving the intervals for each index asc.
	template <typename EnumType>
	static float EnumToNormalizedFloat(EnumType InEnumValue,
	                                   const float Min = 0.f,
	                                   const float Max = 1.f)
	{
		static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

		const UEnum* Enum = StaticEnum<EnumType>();
		if (!ensureMsgf(Enum, TEXT("EnumType must be a reflected UENUM")))
		{
			return Min;
		}

		// UHT-generated enums normally have a final hidden _MAX entry.
		const int32 ValueCount = Enum->NumEnums() - 1;
		if (ValueCount <= 1)
		{
			return Min;
		}

		const int32 EnumIndex = Enum->GetIndexByValue(static_cast<int64>(InEnumValue));
		if (!ensureMsgf(
			EnumIndex != INDEX_NONE && EnumIndex < ValueCount,
			TEXT("Invalid or sentinel enum value")))
		{
			return Min;
		}

		const float Alpha =
			static_cast<float>(EnumIndex + 1) /
			static_cast<float>(ValueCount);

		return FMath::Lerp(Min, Max, Alpha);
	}

	// transform a provided float value into an "enum" normalized float value, mapping the
	// value provided to normalized intervals for each index in the enum type provided
	template <typename EnumType>
	static EnumType RangedFloatToEnum(float InValue,
	                                  const float Min = 0.f,
	                                  const float Max = 1.f)
	{
		static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

		const UEnum* Enum = StaticEnum<EnumType>();
		if (!ensureMsgf(Enum, TEXT("EnumType must be a reflected UENUM")))
		{
			return static_cast<EnumType>(0);
		}

		// Exclude Unreal's generated _MAX sentinel.
		const int32 ValueCount = Enum->NumEnums() - 1;
		if (!ensureMsgf(ValueCount > 0, TEXT("Enum has no usable values")))
		{
			return static_cast<EnumType>(0);
		}

		if (ValueCount == 1 || FMath::IsNearlyEqual(Min, Max))
		{
			return static_cast<EnumType>(Enum->GetValueByIndex(0));
		}

		const float Alpha = FMath::Clamp(
			(InValue - Min) / (Max - Min),
			0.0f,
			1.0f);

		const int32 EnumIndex = FMath::RoundToInt(
			Alpha * static_cast<float>(ValueCount - 1));

		return static_cast<EnumType>(Enum->GetValueByIndex(EnumIndex));
	}
};
