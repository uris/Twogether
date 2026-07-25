// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TogetherGameMode.generated.h"

class ATogetherCharacter;
/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ATogetherGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	ATogetherGameMode();

};
