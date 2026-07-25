// Fill out your copyright notice in the Description page of Project Settings.


#include "TogetherLevelScriptActor.h"

#include "Utility/Debug.h"
#include "TogetherCharacter.h"
#include "Engine/GameInstance.h"
#include "Engine/NetConnection.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATogetherLevelScriptActor::HostLANGame()
{
	// debug message
	PrintDebugMessage(TEXT("Hosting game ..."));

	// start listening for clients
	GetWorld()->ServerTravel("/Game/ThirdPerson/Lvl_ThirdPerson?Listen");
}

void ATogetherLevelScriptActor::JoinLANGame()
{
	// debug message
	PrintDebugMessage(TEXT("Joining game ..."));

	// connect to the game host
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC)
	{
		PC->ClientTravel("192.168.0.211", ETravelType::TRAVEL_Absolute);
	}
}

TArray<ATogetherCharacter*> ATogetherLevelScriptActor::GetTogetherCharacters() const
{
	TArray<AActor*> Actors;
	TArray<ATogetherCharacter*> TogetherCharacterActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATogetherCharacter::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		ATogetherCharacter* TogetherCharacter = Cast<ATogetherCharacter>(Actor);
		if (TogetherCharacter)
		{
			TogetherCharacterActors.Add(TogetherCharacter);
		}
	}

	return TogetherCharacterActors;
}

int32 ATogetherLevelScriptActor::CallServerRPCOnCharacters()
{

	// set up counter
	int32 RPCCallCount = 0;

	// only clients will call server RPC
	if (!HasAuthority())
	{

		// get all characters
		TArray<ATogetherCharacter*> TogetherCharacterActors = GetTogetherCharacters();

		// iterate each character and call RPC
		for (ATogetherCharacter* Character : TogetherCharacterActors)
		{
			Character->ServerRPCFunction(FVector(1.0f, 1.0f, 1.0f));
			RPCCallCount++;
			if (Character->IsLocallyControlled())
			{
				APlayerState* PS = Character->GetPlayerState();
				int32 DisplayName = PS ? PS->GetPlayerId() : -1;
				PrintDebugMessage(FString::Printf(TEXT("Called on %d"), DisplayName));
			}
		}

	}

	// debug message
	PrintDebugMessage(FString::Printf(TEXT("Called %d times"), RPCCallCount));

	// return the number of RPC calls made
	return RPCCallCount;

}

void ATogetherLevelScriptActor::PrintDebugMessage(const FString& DebugMessage) const
{
	if (this->bShowDebugMessages)
	{
		Debug::Print(HasAuthority(),
		             DebugMessage,
		             FGuid::NewGuid(),
		             -1,
		             false,
		             true);
	}
}
