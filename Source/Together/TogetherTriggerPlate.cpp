#include "TogetherTriggerPlate.h"

#include "Utility/Debug.h"
#include "TogetherCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

ATogetherTriggerPlate::ATogetherTriggerPlate()
{
	PrimaryActorTick.bCanEverTick = false;
	this->bReplicates = true;
	SetReplicateMovement(true);

	PlateMesh = GetStaticMeshComponent();

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(PlateMesh);

	TriggerBox->SetBoxExtent(FVector(100.f, 100.f, 50.f));
	TriggerBox->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATogetherTriggerPlate::OnTriggerBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ATogetherTriggerPlate::OnTriggerEndOverlap);
}

void ATogetherTriggerPlate::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (ATogetherCharacter* Character = Cast<ATogetherCharacter>(OtherActor))
	{

		const FString CharacterName = *Character->GetActorNameOrLabel();
		OnTriggerPress(Character, CharacterName);
		// sever is the only one to call client RPCs
		if (HasAuthority())
		{
			Character->ClientRPCFunction(true);
		}
	}

}

void ATogetherTriggerPlate::OnTriggerEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
	)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	if (ATogetherCharacter* Character = Cast<ATogetherCharacter>(OtherActor))
	{
		const FString CharacterName = *Character->GetActorNameOrLabel();
		OnTriggerRelease(Character, CharacterName);
		// sever is the only one to call client RPCs
		if (HasAuthority())
		{
			Character->ClientRPCFunction(false);
		}
	}
}
