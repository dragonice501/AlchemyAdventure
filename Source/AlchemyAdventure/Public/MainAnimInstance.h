// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALCHEMYADVENTURE_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float YawDelta;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float MovementOffsetYaw;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	FRotator RotationLastFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class APawn* Pawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class AMainCharacter* MainCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	class AEnemy* Enemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	float BlockingBlendWeight = 0.f;

public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = "Animation Properties")
	void UpdateAnimationProperties(float DeltaTime);
	
};
