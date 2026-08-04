#pragma once

#include "Misc/Paths.h"
#include "Settings/DeveloperSettings/TogetherSettings.h"

class TOGETHER_API FFileIO
{
public:
	// generic read file
	static FString ReadFile(const FString& FilePath);
};
