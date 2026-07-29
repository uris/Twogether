// Uris Dacpsta All Rights Reserved


#include "TogetherUIPlayerController.h"

#include "Camera/CameraActor.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/UserSettings.h"

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

	// set benchmark results used by settings
	UUserSettings* UserSettings = UUserSettings::Get();
	if (UserSettings->GetLastCPUBenchmarkResult() == -1.0f || UserSettings->GetLastGPUBenchmarkResult() == -1.0f)
	{
		UserSettings->RunHardwareBenchmark();
		UserSettings->ApplyHardwareBenchmarkResults();
	}
}
