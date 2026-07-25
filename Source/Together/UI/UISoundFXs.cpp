// Uris - All Rights Reserved


#include "UISoundFXs.h"

#include "GameplayTagContainer.h"

const FUISoundFXDefinition* UUISoundFXs::FindSoundDefinition(const FGameplayTag& SoundFXTag) const
{
	return SoundFXTag.IsValid() ? SoundFX.Find(SoundFXTag) : nullptr;
}
