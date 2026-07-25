// Copyright Epic Games, Inc. All Rights Reserved.

#include "TogetherCharacter.h"

#include "Utility/Debug.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Together.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

ATogetherCharacter::ATogetherCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATogetherCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATogetherCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATogetherCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATogetherCharacter::Look);
	}
	else
	{
		UE_LOG(LogTogether,
		       Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ),
		       *GetNameSafe(this));
	}
}

void ATogetherCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATogetherCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATogetherCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATogetherCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATogetherCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATogetherCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATogetherCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATogetherCharacter::DoSpawnSphere()
{
	ServerRPCFunction(FVector(1.0f, 1.0f, 1.0f));
}

bool ATogetherCharacter::ServerRPCFunction_Validate(const FVector ScaleXYZ)
{
	return !ScaleXYZ.IsZero();
}

/** Server RPCs execute only on the server */
void ATogetherCharacter::ServerRPCFunction_Implementation(const FVector ScaleXYZ)
{

	// executes only on server but extra check to make sure
	if (HasAuthority())
	{

		// create a UUID
		const FGuid InstanceId = FGuid::NewGuid();

		// is no mesh, don't spawn
		if (!SphereMesh)
		{
			Debug::Print(HasAuthority(), TEXT("No Sphere Mesh"), InstanceId, -1, true, true);
			return;
		}

		// create spawn parameters
		FActorSpawnParameters SpawnParams;

		// set "MyBox" as the spawned actor owner
		SpawnParams.Owner = this;

		// determine spawn location
		const FVector ActorLocation = GetActorLocation();
		const float RandomMultiplier = FMath::RandRange(100.0f, 105.0f);
		const FVector ForwardVector = GetActorRotation().Vector() * RandomMultiplier;
		const FVector UpVector = GetActorUpVector() * 50.f;
		const FVector SpawnLocation = ActorLocation + ForwardVector + UpVector;

		// create a static mesh actor with the spawn params
		AStaticMeshActor* StaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnParams);

		if (StaticMeshActor)
		{
			// set the actor properties
			StaticMeshActor->SetReplicates(true);
			StaticMeshActor->SetReplicateMovement(true);
			StaticMeshActor->SetMobility(EComponentMobility::Movable);
			StaticMeshActor->SetActorLocation(SpawnLocation);
			StaticMeshActor->SetActorRotation(GetActorRotation());
			StaticMeshActor->SetActorScale3D(ScaleXYZ);

			// get static mesh component on the actor
			UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();

			if (StaticMeshComponent)
			{
				// set the mesh properties
				StaticMeshComponent->SetIsReplicated(true);
				StaticMeshComponent->SetSimulatePhysics(true);
				if (SphereMesh)
				{
					StaticMeshComponent->SetStaticMesh(SphereMesh);
				}
			}
			const FString Message = FString::Printf(TEXT("Spawned Sphere Mesh. Scale: %s"), *ScaleXYZ.ToString());
			Debug::Print(HasAuthority(), Message, InstanceId, -1, false, true);
		}
	}
}

void ATogetherCharacter::ClientRPCFunction_Implementation(const bool bIsBurning)
{
	SetBurning(bIsBurning);
}

// spawn the burning effect
void ATogetherCharacter::SetBurning(const bool bIsBurning)
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
			BurningEffectComponent = UGameplayStatics::SpawnEmitterAttached(BurnEffect,
			                                                                GetMesh(),
			                                                                NAME_None,
			                                                                FVector::ZeroVector,
			                                                                FRotator::ZeroRotator,
			                                                                EAttachLocation::KeepRelativeOffset,
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
