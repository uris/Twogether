// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "TogetherLevelScriptActor.generated.h"

class ATogetherCharacter;
/**
 *
 */
UCLASS()
class TOGETHER_API ATogetherLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug Messages")
	bool bShowDebugMessages = true;

	UFUNCTION(BlueprintCallable, Category="Level")
	int32 CallServerRPCOnCharacters();
	void PrintDebugMessage(const FString& DebugMessage) const;

	UFUNCTION(BlueprintCallable, Category="GameMode")
	void HostLANGame();

	UFUNCTION(BlueprintCallable, Category="GameMode")
	void JoinLANGame();

private:
	// get all characters of ATogetherCharacter in the world
	TArray<ATogetherCharacter*> GetTogetherCharacters() const;

};
