#include "SkateChar.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASkateChar::ASkateChar()
{
    // Configuração da câmera (igual ao ThirdPersonCharacter)
    PrimaryActorTick.bCanEverTick = true;

    // Configura movimento do personagem para skate
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->GroundFriction = 0.3f; // Menos atrito (desliza mais)
}

void ASkateChar::BeginPlay()
{
    Super::BeginPlay();

    // Procura o skate no mapa (você vai criar um Actor chamado "Skate" depois)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Skate"), FoundActors);

    if (FoundActors.Num() > 0)
    {
        Skateboard = FoundActors[0]; // Pega o primeiro skate encontrado
    }
}

void ASkateChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Configura controles
    PlayerInputComponent->BindAxis("MoveForward", this, &ASkateChar::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASkateChar::MoveRight);
    PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ASkateChar::Push);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASkateChar::Ollie);
}

// Movimento para frente/trás
void ASkateChar::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

// Movimento para os lados
void ASkateChar::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

// Aceleração (empurrar o skate)
void ASkateChar::Push()
{
    FVector Impulse = GetActorForwardVector() * 50000.0f;
    GetCharacterMovement()->AddImpulse(Impulse, true);
}

// Pulo (Ollie)
void ASkateChar::Ollie()
{
    Jump();
}