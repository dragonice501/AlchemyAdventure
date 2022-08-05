// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayerCharacter.generated.h"

enum class ECameraState : uint8
{
	ECS_Free UMETA(DisplayName = "Free"),
	ECS_Interping UMETA(DisplayName = "Interping"),
	ECS_Fixed UMETA(DisplayName = "Fixed"),
	ECS_Max UMETA(DisplayName = "Default Max"),
};

UCLASS()
class ALCHEMYADVENTURE_API AMainPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* DetectionSphere;

	float RunSpeed = 600.f;
	float WalkSpeed = 250.f;
	float CrouchSpeed = 200.f;
	float ProneSpeed = 70.f;

	bool bMMBPressed = false;
	bool bCrouchPressed = false;
	bool bRunning = false;
	bool bCrouching = false;
	bool bProne = false;
	bool bJumping = false;

	FVector MovementInputVector;

	FTimerHandle CrouchProneTimerHandle;
	float CrouchProneTimerHandleDelay = 0.15f;

	ECameraState CameraState = ECameraState::ECS_Free;
	FRotator CameraFinalInterpRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

private:

public:

	// Sets default values for this character's properties
	AMainPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool GetStrafing() const { return bMMBPressed; }
	FORCEINLINE bool GetRunning() const { return bRunning; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE bool GetProne() const { return bProne; }
	FORCEINLINE bool GetJumping() const { return bJumping; }
	void SetCrouching(bool Crouching) { bCrouching = Crouching; }
	void SetProne(bool Prone) { bProne = Prone; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void JumpPressed();
	void JumpReleased();
	void MMBPressed();
	void MMBReleased();
	void RunPressed();
	void RunReleased();
	void CrouchPressed();
	void CrouchReleased();
	void FirePressed();
	void FireReleased();

	void LookUp(float Value);
	void LookRight(float Value);
	void MoveFoward(float Value);
	void MoveRight(float Value);

	void SetMovementType();
	void SetMovementSpeed();

	void SetCrouchProne();

	void InterpCameraRotation(float DeltaTime);

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
