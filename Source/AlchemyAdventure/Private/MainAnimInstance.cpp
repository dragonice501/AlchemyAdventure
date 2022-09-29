// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "MainCharacter.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		MainCharacter = Cast<AMainCharacter>(Pawn);
		Enemy = Cast<AEnemy>(Pawn);
	}
}

void UMainAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LaterialSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LaterialSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		FRotator AimRotation = Pawn->GetActorRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Pawn->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		FRotator Rotation = Pawn->GetActorRotation();
		FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastFrame, Rotation);
		float Target = Delta.Yaw / DeltaTime;
		float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.0f);
		YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
		RotationLastFrame = Rotation;

		if (MainCharacter)
		{
			bIsAccelerating = MainCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
			BlockingBlendWeight = MainCharacter->bBlocking ? 1.0f : 0.f;
		}
		else if (Enemy)
		{
			bIsAccelerating = Enemy->bIsAccelerating;
		}
	}
}