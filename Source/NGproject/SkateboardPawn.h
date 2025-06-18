// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SkateboardPawn.generated.h"

UCLASS()
class NGPROJECT_API ASkateboardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASkateboardPawn();

	// Move Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void StartPush();
	void StopPush();
	void Jump();

	UFUNCTION()
	void OnSkateboardHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse,
		const FHitResult& Hit);

	UPROPERTY(BlueprintReadOnly, Category = "Score")
	int32 TotalScore = 0;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* SkateboardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* Camera;

	// Movement Variables
	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float PushBoost = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpForce = 600.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	FVector CurrentVelocity;
	FVector MovementInput;
	bool bIsPushing;
	float CurrentMoveSpeed;

	// Controles de camera
	void LookUp(float Value);
	void Turn(float Value);

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraPitchSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float CameraYawSpeed = 20.0f;

};
