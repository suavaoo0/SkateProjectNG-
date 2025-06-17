#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkateChar.generated.h"

UCLASS()
class SKATE_API ASkateChar : public ACharacter
{
    GENERATED_BODY()

public:
    ASkateChar(); // Construtor

    // Variável para armazenar o skate
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skate")
    class AActor* Skateboard;

    // Funções de movimento
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Push();  // Acelera o skate
    void Ollie(); // Pulo (Ollie)

protected:
    virtual void BeginPlay() override;

public:
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};