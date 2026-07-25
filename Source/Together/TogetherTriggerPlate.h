// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "TogetherTriggerPlate.generated.h"

class ATogetherCharacter;
class UBoxComponent;
class UPrimitiveComponent;
class UStaticMeshComponent;

/**
 *
 */
UCLASS()
class TOGETHER_API ATogetherTriggerPlate : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ATogetherTriggerPlate();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	UFUNCTION()
	void OnTriggerBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	UFUNCTION()
	void OnTriggerEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	UFUNCTION(BlueprintImplementableEvent, Category="Trigger Plate Events")
	void OnTriggerPress(ATogetherCharacter* Character, const FString& CharacterName);

	UFUNCTION(BlueprintImplementableEvent, Category="Trigger Plate Events")
	void OnTriggerRelease(ATogetherCharacter* Character, const FString& CharacterName);
};
