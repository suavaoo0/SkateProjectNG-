// Fill out your copyright notice in the Description page of Project Settings.


#include "SkateboardPawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
ASkateboardPawn::ASkateboardPawn()
{
 	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// Load Static Mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SkateMesh(TEXT("/Game/Skate/Skateboard.Skateboard")); // Mesh path
	SkateboardMesh = CreateDefaultSubobject<UStaticMeshComponent>("SkateboardMesh");
	if (SkateMesh.Succeeded())
	{
		SkateboardMesh->SetStaticMesh(SkateMesh.Object);
	}
	RootComponent = SkateboardMesh;

	// Habilita física (INSIRA isso após criar o SkateboardMesh)
	SkateboardMesh->SetSimulatePhysics(true);
	SkateboardMesh->SetLinearDamping(0.2f); // Controla a inércia
	SkateboardMesh->BodyInstance.bLockRotation = true; // Evita tombamento

	// Create a Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 500.f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

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

    if (!CurrentVelocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		SetActorLocation(NewLocation, true);
	}

}

// Implementação do movimento
void ASkateboardPawn::MoveForward(float Value)
{
	CurrentVelocity.X = Value * MoveSpeed;
}

void ASkateboardPawn::MoveRight(float Value)
{
	CurrentVelocity.Y = Value * MoveSpeed;
}

void ASkateboardPawn::StartPush()
{
	bIsPushing = true;
	MoveSpeed += PushBoost;
}

void ASkateboardPawn::StopPush()
{
	bIsPushing = false;
	MoveSpeed -= PushBoost;
}

void ASkateboardPawn::Jump()
{
	if (SkateboardMesh->IsSimulatingPhysics())
	{
		SkateboardMesh->AddImpulse(FVector(0, 0, JumpForce), NAME_None, true);
	}
}

// Called to bind functionality to input
void ASkateboardPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movimento contínuo (WASD)
	PlayerInputComponent->BindAxis("MoveForward", this, &ASkateboardPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASkateboardPawn::MoveRight);

	// Ações (teclas configuradas)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASkateboardPawn::Jump); // Barra de Espaço
	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ASkateboardPawn::StartPush); // Tecla Q (pressionar)
	PlayerInputComponent->BindAction("Push", IE_Released, this, &ASkateboardPawn::StopPush); // Tecla Q (soltar)

}

