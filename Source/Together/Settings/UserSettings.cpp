// Uris - All Rights Reserved


#include "UserSettings.h"

#include "Engine/Engine.h"

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
