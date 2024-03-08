// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	character = Cast<AMyCharacter>(TryGetPawnOwner());
	if (character)
	{
		movementComponent = character->GetCharacterMovement();
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (movementComponent)
	{
		groundSpeed = UKismetMathLibrary::VSizeXY(movementComponent->Velocity);

		isFalling = movementComponent->IsFalling();
	}
}
