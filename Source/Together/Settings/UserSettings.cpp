// Uris - All Rights Reserved


#include "UserSettings.h"

#include "Engine/Engine.h"

UUserSettings::UUserSettings() : OverallVolume(1.0f) {}

UUserSettings* UUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

FString UUserSettings::GetGameDifficulty() const
{
	return GameDifficulty;
}

void UUserSettings::SetGameDifficulty(const FString& InDifficulty)
{
	GameDifficulty = InDifficulty;
}

float UUserSettings::GetOverallVolume() const
{
	return OverallVolume;
}

void UUserSettings::SetOverallVolume(const float InVolume)
{
	OverallVolume = InVolume;
	// add logic for setting overall volume later
}
