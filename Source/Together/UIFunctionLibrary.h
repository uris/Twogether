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

	UFUNCTION(BlueprintPure, Category = "Editor Preview")
	static FText FormatRichText(const FString& Input);
	static FText FormatRichText(const FText& Input);

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

		const TArray<int64> UsableValues = GetUsableEnumValues<EnumType>();
		if (UsableValues.IsEmpty())
		{
			return Min;
		}

		const int32 EnumIndex = UsableValues.IndexOfByKey(
			static_cast<int64>(InEnumValue));
		if (!ensureMsgf(
			EnumIndex != INDEX_NONE,
			TEXT("Invalid, custom, hidden, or sentinel enum value")))
		{
			return Min;
		}

		if (UsableValues.Num() == 1)
		{
			return Max;
		}

		const float Alpha =
			static_cast<float>(EnumIndex + 1) /
			static_cast<float>(UsableValues.Num());

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

		const TArray<int64> UsableValues = GetUsableEnumValues<EnumType>();
		if (!ensureMsgf(!UsableValues.IsEmpty(), TEXT("Enum has no usable values")))
		{
			return static_cast<EnumType>(0);
		}

		if (UsableValues.Num() == 1 || FMath::IsNearlyEqual(Min, Max))
		{
			return static_cast<EnumType>(UsableValues[0]);
		}

		const float Alpha = FMath::Clamp(
			(InValue - Min) / (Max - Min),
			0.0f,
			1.0f);

		// Real enum entries occupy positive steps through the range; zero is
		// intentionally not assigned to an entry. For example, five entries
		// map to .2, .4, .6, .8, and 1.0.
		const int32 EnumIndex = FMath::Clamp(
			FMath::RoundToInt(Alpha * static_cast<float>(UsableValues.Num())) - 1,
			0,
			UsableValues.Num() - 1);

		return static_cast<EnumType>(UsableValues[EnumIndex]);
	}

private:
	template <typename EnumType>
	static TArray<int64> GetUsableEnumValues()
	{
		static_assert(TIsEnum<EnumType>::Value, "EnumType must be an enum");

		TArray<int64> UsableValues;
		const UEnum* Enum = StaticEnum<EnumType>();
		if (!Enum)
		{
			return UsableValues;
		}

		for (int32 EnumIndex = 0; EnumIndex < Enum->NumEnums(); ++EnumIndex)
		{
			const int64 EnumValue = Enum->GetValueByIndex(EnumIndex);

			// -1 is reserved for display-only entries such as Custom.
			if (EnumValue == -1)
			{
				continue;
			}

			// Exclude Unreal's generated terminal sentinel.
			if (EnumIndex == Enum->NumEnums() - 1 &&
			    EnumValue == Enum->GetMaxEnumValue())
			{
				continue;
			}

#if WITH_METADATA
			if (Enum->HasMetaData(TEXT("Hidden"), EnumIndex))
			{
				continue;
			}
#endif

			UsableValues.Add(EnumValue);
		}

		return UsableValues;
	}
};
