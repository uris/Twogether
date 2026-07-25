#include "MyBox.h"

#include "Utility/Debug.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"

// Set defaults
AMyBox::AMyBox()
{
	// No need to call tick for this actor
	PrimaryActorTick.bCanEverTick = false;

	// Actor will implement replication
	bReplicates = true;
}

// Implement replication
void AMyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// parent replication method call
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate InstanceId
	DOREPLIFETIME(AMyBox, InstanceId);
}

// Called when the game starts or when spawned
void AMyBox::BeginPlay()
{
	// call parent method
	Super::BeginPlay();

	// set replicate movement to true
	SetReplicateMovement(true);

	// detect authority from the actor's role - can alo use GetLocalRole() == ROLE_Authority;
	bHasAuthority = HasAuthority();

	// set authority message text value
	AuthorityTextValue = bHasAuthority
		                     ? FText::FromString(TEXT("Authority: True"))
		                     : FText::FromString(TEXT("Authority: False"));

	// Only the server creates the UUID; clients receive it through replication.
	if (bHasAuthority && !InstanceId.IsValid())
	{

		InstanceId = FGuid::NewGuid(); // set the instance id

		AActor::ForceNetUpdate(); // force replication of the actor

		OnRep_InstanceId(); // manually trigger the replication method on the server (called automatically on clients)
	}

	// create the callback lambda running the multicast RPC call
	if (bHasAuthority)
	{
		const TFunction<void()> SetBurningCallback = ([this]()
		{
			MulticastRPCFunction(true);
		});

		const TFunction<void()> ClearBurningCallback = ([this]()
		{
			MulticastRPCFunction(false);
		});

		// start an interval to trigger the multicast RPC callback with burn set to true
		BurnInterval = FWorldInterval::SetInterval(this, SetBurningCallback, 1.0f, 10.0f, false);

		// start an interval to trigger the multicast RPC callback with burn set to false 10 seconds later
		ClearBurnInterval = FWorldInterval::SetInterval(this, ClearBurningCallback, 1.0f, 15.0f, false);
	}

	// trigger initialization complete event
	OnInitializationComplete();
}

/** REPLICATION */

void AMyBox::OnRep_InstanceId() const
{
	if (bShowReplicationMessage)
	{
		PrintReplicationMessage();
	}

	if (bShowAuthorityMessage)
	{
		PrintAuthorityMessage();
	}
}

/** RPC CALLS */

void AMyBox::MulticastRPCFunction_Implementation(const bool bIsBurning)
{
	FWorldInterval::ClearInterval(bIsBurning ? BurnInterval : ClearBurnInterval);
	SetBurning(bIsBurning);
	PrintMulticastRPCMessage(bIsBurning);
}

/** EFFECTS */

void AMyBox::SetBurning(const bool bIsBurning)
{

	// exit if this is a server only i.e., with no client so no need for effects
	if (IsRunningDedicatedServer())
	{
		return;
	}

	// if burning trigger the burning effect
	if (bIsBurning)
	{
		if (BurnEffect && !IsValid(BurningEffectComponent))
		{
			BurningEffectComponent = UGameplayStatics::SpawnEmitterAtLocation(this,
			                                                                  BurnEffect,
			                                                                  GetActorLocation(),
			                                                                  FRotator::ZeroRotator,
			                                                                  false,
			                                                                  EPSCPoolMethod::None,
			                                                                  true);
		}
	}
	// otherwise destroy the burning effect
	else
	{
		if (IsValid(BurningEffectComponent))
		{
			BurningEffectComponent->DestroyComponent();
			BurningEffectComponent = nullptr;
		}
	}
}

/** DEBUG MESSAGES */

void AMyBox::PrintReplicationMessage() const
{
	const FString Message = TEXT("Replicated InstanceId");
	Debug::Print(HasAuthority(), Message, InstanceId, -1, false, true);
}

void AMyBox::PrintAuthorityMessage() const
{
	const FString HasAuth = HasAuthority() ? TEXT("Yes") : TEXT("No");
	const FString Message = FString::Printf(TEXT("Has Authority: %s"), *HasAuth);
	Debug::Print(HasAuthority(), Message, InstanceId, -1, false, true);
}

void AMyBox::PrintMulticastRPCMessage(const bool bActive) const
{
	const FString Message = bActive
		                        ? TEXT("Explosion Multicast RPC called")
		                        : TEXT("Explosion Multicast RPC calls ended");
	Debug::Print(HasAuthority(), Message, InstanceId, -1, false, true);
}
