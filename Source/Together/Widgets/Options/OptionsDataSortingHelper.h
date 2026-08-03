// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"

struct FUserSettingDefinition;

/** Builds the display order for setting definitions while preserving their authored hierarchy. */
class FOptionsDataSortingHelper final
{
public:
	static void SortDefinitionsByHierarchy(TArray<const FUserSettingDefinition*>& Definitions);
};
