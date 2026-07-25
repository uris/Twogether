#pragma once

#include "CoreMinimal.h"
#include "Utility/WorldInterval.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "MyBox.generated.h"

UCLASS()
class TOGETHER_API AMyBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyBox();

	// Register properties replicated by this actor.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	/** Properties */

	// store authority value for qol
	UPROPERTY(BlueprintReadOnly, Category = "Box Properties")
	bool bHasAuthority = false;

	// user-friendly authority label text accessible in derived BPs
	UPROPERTY(BlueprintReadOnly, Category = "Box Properties")
	FText AuthorityTextValue = FText::FromString(TEXT("Authority: Unknown"));

	// box explosion particle effect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Properties")
	UParticleSystem* BurnEffect = nullptr;

	// reference to the burn effect component
	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> BurningEffectComponent;

	/** Lifecycle */

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// BP callable event triggered when begin play code has run
	UFUNCTION(BlueprintImplementableEvent, Category = "Box Events")
	void OnInitializationComplete();

	/** Multicast RPC function */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFunction(bool bIsBurning = false);

private:
	// show an authority debug message
	UPROPERTY(EditInstanceOnly, Category = "Box Debug Options")
	bool bShowAuthorityMessage = false;

	// show a replication event trigger message
	UPROPERTY(EditInstanceOnly, Category = "Box Debug Options")
	bool bShowReplicationMessage = false;

	// UUID for each instance
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_InstanceId, Category = "Box Debug Options")
	FGuid InstanceId;

	// timers for burning
	FWorldIntervalPtr BurnInterval;
	FWorldIntervalPtr ClearBurnInterval;

	// process replication notification (internal to class)
	UFUNCTION()
	void OnRep_InstanceId() const;

	// Set / Remove Burning
	UFUNCTION()
	void SetBurning(bool bIsBurning);

	/** Internal Debugging */
	void PrintAuthorityMessage() const;
	void PrintMulticastRPCMessage(bool bActive) const;
	void PrintReplicationMessage() const;
};
