// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "GameFramework/SpringArmComponent.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	capsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	capsuleComponent->SetCapsuleHalfHeight(20.0f);
	capsuleComponent->SetCapsuleRadius(15.0f);
	SetRootComponent(capsuleComponent);

	skeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Static Mesh Component"));
	skeletalMeshComponent->SetupAttachment(GetRootComponent());

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	springArmComponent->SetupAttachment(GetRootComponent());
	springArmComponent->TargetArmLength = 200.0f;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	cameraComponent->SetupAttachment(springArmComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* subsystem =
			ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subsystem->AddMappingContext(mappingContext, 0);
		}
	}
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent =
		CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(moveAction, ETriggerEvent::Triggered, this, &AMyPawn::MoveForward);
	}

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyPawn::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyPawn::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyPawn::LookUp);
}

void AMyPawn::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMyPawn::MoveForward(const FInputActionValue& Value)
{
	const float directionValue = Value.Get<float>();

	if (Controller && directionValue != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), directionValue);
	}
}

void AMyPawn::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMyPawn::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

