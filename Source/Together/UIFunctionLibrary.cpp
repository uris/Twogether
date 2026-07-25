// Uris - All Rights Reserved


#include "UIFunctionLibrary.h"

#include "GameplayTagsManager.h"
#include "Engine/Engine.h"
#include "Settings/TogetherSettings.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/UIIconSet.h"
#include "Subsystems/UI/UISubsystem.h"
#include "Utility/Debug.h"

TSoftClassPtr<UWidget_ActivatableBase> UUIFunctionLibrary::GetFrontEndSoftWidgetClassByTag(FGameplayTag InWidgetTag)
{
	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
	checkf(Settings->UIWidgetMap.Contains(InWidgetTag),
	       TEXT("Could not find Widget Tag %s"),
	       *InWidgetTag.ToString());

	return Settings->UIWidgetMap.FindRef(InWidgetTag);
}

bool UUIFunctionLibrary::FindUIIconDefinition(const FGameplayTag IconTag,
                                              FUIIconDefinition& OutIconDefinition)
{
	if (!IconTag.IsValid())
	{
		return false;
	}

	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
	const UUIIconSet* IconSet = Settings->DefaultIconSet.LoadSynchronous();
	if (!IconSet)
	{
		return false;
	}

	if (const FUIIconDefinition* Definition = IconSet->FindIcon(IconTag))
	{
		OutIconDefinition = *Definition;
		return true;
	}

	return false;
}

bool UUIFunctionLibrary::FindSoundFXDefinition(const FGameplayTag SoundFXTag, FUISoundFXDefinition& OutSoundDefinition)
{
	if (!SoundFXTag.IsValid())
	{
		return false;
	}

	const UTogetherSettings* Settings = GetDefault<UTogetherSettings>();
	const UUISoundFXs* SoundFX = Settings->DefaultSoundFX.LoadSynchronous();
	if (!SoundFX)
	{
		return false;
	}

	if (const FUISoundFXDefinition* Definition = SoundFX->FindSoundDefinition(SoundFXTag))
	{
		OutSoundDefinition = *Definition;
		return true;
	}

	return false;
}

// helper: get all gameplay tags under a specific tag
TArray<FGameplayTag> UUIFunctionLibrary::GetLeafChildTags(const FGameplayTag ParentTag)
{
	TArray<FGameplayTag> TerminalTags;
	const UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

	// all tags matching parent
	FGameplayTagContainer AllChildren;
	TagManager.RequestAllGameplayTags(AllChildren, false);

	// weed out ones with children
	for (const FGameplayTag& Tag : AllChildren)
	{
		// is in parent
		if (Tag.MatchesTag(ParentTag))
		{
			// has no children
			TSharedPtr<FGameplayTagNode> TagNode = TagManager.FindTagNode(Tag);
			if (TagNode.IsValid() && TagNode->GetChildTagNodes().Num() == 0)
			{
				TerminalTags.Add(Tag);
			}
		}
	}

	return TerminalTags;
}

void UUIFunctionLibrary::PlaySoundFX(const UObject* Context,
                                     const FGameplayTag SoundFXTag,
                                     const float DesiredSoundLevel)
{
	// check engine
	if (!GEngine)
	{
		return;
	}

	// get world context
	const UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull);
	if (!World)
	{
		return;
	}

	// get ui subsystem
	const UUISubsystem* UI = World->GetGameInstance()->GetSubsystem<UUISubsystem>();
	if (!UI)
	{
		return;
	}

	// play sound
	if (USoundBase* Sound = UI->GetPreloadedSFX(SoundFXTag))
	{
		UGameplayStatics::PlaySound2D(World, Sound, DesiredSoundLevel);
	}

}
