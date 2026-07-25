#pragma once

#include "CoreMinimal.h"

class TOGETHER_API Debug
{
public:
	static void Print(
		const FString& Message,
		const float Duration = 15.0f,
		int32 MessageKey = -1);

	static void Print(
		bool bIsServer,
		const FString& Message,
		const FGuid& InstanceId,
		int32 MessageKey = -1,
		bool bIsError = false,
		bool bPersists = true);
};
