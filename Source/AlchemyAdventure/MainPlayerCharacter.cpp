// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AMainPlayerCharacter::AMainPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Detection Sphere"));
	DetectionSphere->SetupAttachment(RootComponent);
	DetectionSphere->SetSphereRadius(500.f);
}

// Called when the game starts or when spawned
void AMainPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	float num = 6.283;

	UE_LOG(LogTemp, Warning, TEXT("Floor: %f"), FMath::Floor(num));
	UE_LOG(LogTemp, Warning, TEXT("Ceil: %f"), FMath::CeilToFloat(num));
	UE_LOG(LogTemp, Warning, TEXT("Round: %f"), FMath::RoundToFloat(num));

	//int in = FMath::Pow(2, 24);

	//UE_LOG(LogTemp, Warning, TEXT("Pow: %d"), in);

	DetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainPlayerCharacter::OnSphereBeginOverlap);
}

// Called every frame
void AMainPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CameraState == ECameraState::ECS_Interping) InterpCameraRotation(DeltaTime);

	FRotator CharacterRotation = GetActorRotation();
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		1,
		-1,
		FColor::Orange,
		FString::Printf(TEXT("Character Rotation: %f, %f, %f"), CharacterRotation.Pitch, CharacterRotation.Yaw, CharacterRotation.Roll));

	FRotator DesiredRelativeMovementRotation = MovementInputVector.Rotation();
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		2,
		-1,
		FColor::Orange,
		FString::Printf(TEXT("Desired Relative Movement Rotation: %f, %f, %f"), DesiredRelativeMovementRotation.Pitch, DesiredRelativeMovementRotation.Yaw, 0));

	FRotator ControllerRotation = GetControlRotation();
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		3,
		-1,
		FColor::Orange,
		FString::Printf(TEXT("Controller Rotation: %f, %f, %f"), ControllerRotation.Pitch, ControllerRotation.Yaw, ControllerRotation.Roll));

	if (GEngine) GEngine->AddOnScreenDebugMessage(
		4,
		-1,
		FColor::Orange,
		FString::Printf(TEXT("Movement Style: %s"), bMMBPressed ? TEXT("Strafing") : TEXT("Orientation")));
}

// Called to bind functionality to input
void AMainPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("MMB", EInputEvent::IE_Pressed, this, &AMainPlayerCharacter::MMBPressed);
	PlayerInputComponent->BindAction("MMB", EInputEvent::IE_Released, this, &AMainPlayerCharacter::MMBReleased);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &AMainPlayerCharacter::RunPressed);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &AMainPlayerCharacter::RunReleased);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AMainPlayerCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &AMainPlayerCharacter::CrouchReleased);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AMainPlayerCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &AMainPlayerCharacter::JumpReleased);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AMainPlayerCharacter::FirePressed);
	PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AMainPlayerCharacter::FireReleased);

	PlayerInputComponent->BindAxis("LookUp", this, &AMainPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis("LookRight", this, &AMainPlayerCharacter::LookRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayerCharacter::MoveFoward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayerCharacter::MoveRight);
}

void AMainPlayerCharacter::MMBPressed()
{
	bMMBPressed = true;
	SetMovementType();
}

void AMainPlayerCharacter::MMBReleased()
{
	bMMBPressed = false;
	SetMovementType();
}

void AMainPlayerCharacter::RunPressed()
{
	bRunning = true;
	SetMovementSpeed();
}

void AMainPlayerCharacter::RunReleased()
{
	bRunning = false;
	SetMovementSpeed();
}

void AMainPlayerCharacter::CrouchPressed()
{
	bCrouchPressed = true;
	GetWorldTimerManager().SetTimer(CrouchProneTimerHandle, this, &AMainPlayerCharacter::SetCrouchProne, CrouchProneTimerHandleDelay);
}

void AMainPlayerCharacter::CrouchReleased()
{
	bCrouchPressed = false;
}

void AMainPlayerCharacter::JumpPressed()
{
	Jump();
	bJumping = true;
	bRunning = false;
	bCrouching = false;
	bProne = false;
}

void AMainPlayerCharacter::JumpReleased()
{
	bJumping = false;
}

void AMainPlayerCharacter::FirePressed()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fire"));
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(FName("Fire"));
	}
}

void AMainPlayerCharacter::FireReleased()
{

}

void AMainPlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMainPlayerCharacter::LookRight(float Value)
{
	AddControllerYawInput(Value);
}

void AMainPlayerCharacter::MoveFoward(float Value)
{
	MovementInputVector.X = Value;

	if (Controller && FMath::Abs(Value) >= 0.25f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector ForwardDirection{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::X) };
		AddMovementInput(ForwardDirection, Value);
	}
}

void AMainPlayerCharacter::MoveRight(float Value)
{
	MovementInputVector.Y = Value;

	if (Controller && FMath::Abs(Value) >= 0.25f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

		const FVector RightDirection{ FRotationMatrix{YawRotation}.GetUnitAxis(EAxis::Y) };
		AddMovementInput(RightDirection, Value);
	}
}

void AMainPlayerCharacter::SetMovementType()
{
	if (bMMBPressed)
	{
		//SetActorRotation(FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f));
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}

	/*if (GetControlRotation().Yaw != GetActorRotation().Yaw)
	{
		CameraState = ECameraState::ECS_Interping;
	}*/
}

void AMainPlayerCharacter::SetMovementSpeed()
{
	if (bRunning)GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	else if (bCrouching) GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	else if (bProne) GetCharacterMovement()->MaxWalkSpeed = ProneSpeed;
	else GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMainPlayerCharacter::SetCrouchProne()
{
	if (bCrouchPressed)
	{
		if (bCrouching)
		{
			bProne = true;
			bCrouching = false;
		}
		else if (bProne)
		{
			bProne = false;
			bCrouching = false;
		}
		else
		{
			bProne = true;
			bCrouching = false;
		}
	}
	else
	{
		if (bCrouching)
		{
			bCrouching = false;
		}
		else
		{
			bProne = false;
			bCrouching = true;
		}
	}

	SetMovementSpeed();
}

void AMainPlayerCharacter::InterpCameraRotation(float DeltaTime)
{
	float CurrentCameraYaw = GetControlRotation().Yaw;
	CurrentCameraYaw = FMath::FInterpTo(CurrentCameraYaw, GetActorRotation().Yaw, DeltaTime, 20.f);
	Controller->SetControlRotation(FRotator(0.f, CurrentCameraYaw, 0.f));

	if (CurrentCameraYaw < 1.f) CameraState = ECameraState::ECS_Free;
}

void AMainPlayerCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
	}
}

void AMainPlayerCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}