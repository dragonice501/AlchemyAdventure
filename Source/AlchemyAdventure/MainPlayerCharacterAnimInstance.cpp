// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerCharacterAnimInstance.h"
#include "MainPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UMainPlayerCharacterAnimInstance::UMainPlayerCharacterAnimInstance()
{

}

void UMainPlayerCharacterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MainPlayerCharacter)
	{
		FVector Velocity = MainPlayerCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		Strafing = MainPlayerCharacter->GetStrafing();
		Running = MainPlayerCharacter->GetRunning();
		Crouching = MainPlayerCharacter->GetCrouching();
		Prone = MainPlayerCharacter->GetProne();
		Jumping = MainPlayerCharacter->GetCharacterMovement()->IsFalling() ? true : false;

		if (Running && Speed > 0) MainPlayerCharacter->SetCrouching(false);

		FRotator AimRotation = MainPlayerCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MainPlayerCharacter->GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

		
	}
}

void UMainPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	MainPlayerCharacter = Cast<AMainPlayerCharacter>(TryGetPawnOwner());
}