// Uris - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TogetherUIPlayerController.generated.h"

/**
 *
 */
UCLASS()
class TOGETHER_API ATogetherUIPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	/** Called when the player starts possessing a pawn */
	virtual void OnPossess(APawn* InPawn) override;

};
