// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"



ASkateboardPawn::ASkateboardPawn()
{
 	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Mesh Setup
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkateMesh(TEXT("/Game/Skate/Skateboard.Skateboard")); // Mesh path
	SkateboardMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkateboardMesh");
	if (SkateMesh.Succeeded())
	{
		SkateboardMesh->SetStaticMesh(SkateMesh.Object);
	}
	RootComponent = SkateboardMesh;

	// Physics setup
	SkateboardMesh->SetSimulatePhysics(true);
	SkateboardMesh->SetLinearDamping(0.3f);
	SkateboardMesh->SetAngularDamping(0.5f);
	SkateboardMesh->BodyInstance.bLockRotation = true;
	SkateboardMesh->SetNotifyRigidBodyCollision(true);

	// Create a Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Init variables
	CurrentMoveSpeed = MoveSpeed;
	bIsPushing = false;

}


// Called when the game starts or when spawned
void ASkateboardPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASkateboardPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementInput.IsZero())
	{
		// Get camera forward vector (ignoring pitch)
		FRotator CameraRot = GetControlRotation();
		CameraRot.Pitch = 0;
		CameraRot.Roll = 0;

		FVector Direction = CameraRot.RotateVector(FVector(MovementInput.X, MovementInput.Y, 0));
		Direction.Normalize();

		// Apply physical force
		SkateboardMesh->AddForce(Direction * CurrentMoveSpeed * DeltaTime * 1000.f, NAME_None, true);
	}
}



// Implementação do movimento
void ASkateboardPawn::MoveForward(float Value)
{
	MovementInput.X = Value;
}

void ASkateboardPawn::MoveRight(float Value)
{
	MovementInput.Y = Value;
}

void ASkateboardPawn::StartPush()
{
	bIsPushing = true;
	CurrentMoveSpeed = MoveSpeed + PushBoost;
}

void ASkateboardPawn::StopPush()
{
	bIsPushing = false;
	CurrentMoveSpeed = MoveSpeed;
}

void ASkateboardPawn::Jump()
{
	if (SkateboardMesh->IsSimulatingPhysics())
	{
		// Simple ground check
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() - FVector(0, 0, 50), ECC_Visibility, Params))
		{
			SkateboardMesh->AddImpulse(FVector(0, 0, JumpForce), NAME_None, true);
		}
	}
}


void ASkateboardPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// inputs
	PlayerInputComponent->BindAxis("MoveForward", this, &ASkateboardPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASkateboardPawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASkateboardPawn::Jump);
	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ASkateboardPawn::StartPush);
	PlayerInputComponent->BindAction("Push", IE_Released, this, &ASkateboardPawn::StopPush);

}

