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
	SkateboardMesh->BodyInstance.bLockRotation = false;
	SkateboardMesh->BodyInstance.bLockXRotation = true;
	SkateboardMesh->BodyInstance.bLockYRotation = true;

	// Create a Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Init variables
	CurrentMoveSpeed = MoveSpeed;
	bIsPushing = false;

	SpringArm->bUsePawnControlRotation = false; // Importante!
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	// Camera Control
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0, 0)); // Ângulo inicial
	SpringArm->CameraLagSpeed = 3.0f; // Para suavizar o movimento

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


	// Aplica movimento se houver input
	if (!MovementInput.IsZero())
	{
		// Obtém a rotação atual do skate
		FRotator SkateRotation = SkateboardMesh->GetComponentRotation();

		// Calcula a direção do movimento relativa ao skate
		FVector Direction = SkateRotation.RotateVector(FVector(MovementInput.X, MovementInput.Y, 0));
		Direction.Normalize();

		// Aplica força na direção calculada
		SkateboardMesh->AddForce(
			Direction * CurrentMoveSpeed * DeltaTime * 600.f,
			NAME_None,
			true
		);
	}

	{
		// Alinha a rotação do skate com a direção do movimento
		if (!SkateboardMesh->GetPhysicsLinearVelocity().IsNearlyZero(10.0f))
		{
			FRotator TargetRotation = SkateboardMesh->GetPhysicsLinearVelocity().Rotation();
			TargetRotation.Pitch = 0;
			TargetRotation.Roll = 0;

			FRotator CurrentRotation = SkateboardMesh->GetComponentRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);

			SkateboardMesh->SetWorldRotation(NewRotation);
		}
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

	if (Value != 0.0f && SkateboardMesh->IsSimulatingPhysics())
	{
		float TurnSpeed = 10.0f; // Reduzi de 50 para 30 (menos sensível)
		SkateboardMesh->AddTorqueInDegrees(
			FVector(0, 0, Value * TurnSpeed * SkateboardMesh->GetMass()),
			NAME_None,
			true
		);
	}
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

void ASkateboardPawn::LookUp(float Value)
{
	if (Value != 0.0f)
	{
		// Apenas inclina a câmera (vertical)
		float NewPitch = SpringArm->GetRelativeRotation().Pitch + (Value * CameraPitchSpeed * GetWorld()->GetDeltaSeconds());
		NewPitch = FMath::Clamp(NewPitch, -80.0f, 80.0f); // Limita o ângulo
		SpringArm->SetRelativeRotation(FRotator(NewPitch, 0, 0));
	}
}

void ASkateboardPawn::Turn(float Value)
{
	if (Value != 0.0f)
	{
		// Apenas rotaciona a câmera horizontalmente
		float NewYaw = SpringArm->GetRelativeRotation().Yaw + (Value * CameraYawSpeed * GetWorld()->GetDeltaSeconds());
		SpringArm->SetRelativeRotation(FRotator(SpringArm->GetRelativeRotation().Pitch, NewYaw, 0));
	}
}


void ASkateboardPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movimento
	PlayerInputComponent->BindAxis("MoveForward", this, &ASkateboardPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASkateboardPawn::MoveRight);

	// Ações
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASkateboardPawn::Jump);
	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ASkateboardPawn::StartPush);
	PlayerInputComponent->BindAction("Push", IE_Released, this, &ASkateboardPawn::StopPush);

	// Controles de câmera
	PlayerInputComponent->BindAxis("LookUp", this, &ASkateboardPawn::LookUp);
	PlayerInputComponent->BindAxis("Turn", this, &ASkateboardPawn::Turn);

}

