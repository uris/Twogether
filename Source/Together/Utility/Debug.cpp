#include "Debug.h"

#include "Engine/Engine.h"
#include "Settings/TogetherSettings.h"

void Debug::Print(
	const FString& Message,
	const float Duration,
	const int32 MessageKey)
{

	const bool bShowDebug = !GetDefault<UTogetherSettings>()->bGlobalHideDebugMessages;
	if (GEngine && bShowDebug)
	{
		GEngine->AddOnScreenDebugMessage(
			MessageKey,
			Duration,
			FColor::Green,
			Message);
	}
}

void Debug::Print(
	const bool bIsServer,
	const FString& Message,
	const FGuid& InstanceId,
	const int32 MessageKey,
	const bool bIsError,
	const bool bPersists)
{
	const float Duration = bPersists ? 5000.0f : 5.0f;
	const bool bShowDebug = !GetDefault<UTogetherSettings>()->bGlobalHideDebugMessages;

	FColor MsgColor = bIsServer ? FColor::Blue : FColor::Green;
	if (bIsError)
	{
		MsgColor = FColor::Red;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			MessageKey,
			Duration,
			MsgColor,
			FString::Printf(
				TEXT("%s :: %s"),
				*Message,
				*InstanceId.ToString()));
	}
}
