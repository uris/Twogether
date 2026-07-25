// Uris - All Rights Reserved

#include "UI/UIIconSet.h"

const FUIIconDefinition* UUIIconSet::FindIcon(const FGameplayTag& IconTag) const
{
	return IconTag.IsValid() ? Icons.Find(IconTag) : nullptr;
}
