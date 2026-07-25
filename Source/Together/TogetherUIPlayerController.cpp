// Uris Dacpsta All Rights Reserved


#include "TogetherUIPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

void ATogetherUIPlayerController::OnPossess(APawn* InPawn)
{
	// call parent
	Super::OnPossess(InPawn);

	// get default cameras
	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACameraActor::StaticClass(), FName("Default"), Cameras);

	if (!Cameras.IsEmpty())
	{
		SetViewTarget(Cameras[0]);
	}
}
