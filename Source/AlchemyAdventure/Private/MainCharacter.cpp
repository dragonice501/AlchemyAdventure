// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "MainPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Pickup.h"
#include "Item.h"
#include "Weapon.h"
#include "Resource.h"
#include "Usable.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	EnemyDetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyDetectionSphere"));
	EnemyDetectionSphere->SetupAttachment(RootComponent);

	SwitchMovementStyle(EMovementStyle::EMS_Free);
	SwitchCameraMovement(ECameraMovement::ECM_Free);

	Health = 85.f;
	MaxHealth = 100.f;
	XP = 0;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnemyDetectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::EnemyDetectionSphereBeginOverlap);
	EnemyDetectionSphere->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::EnemyDetectionSphereEndOverlap);

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	if (StartingUsables.Num() > 0)
	{
		for (int i = 0; i < StartingUsables.Num(); i++)
		{
			AUsable* Usable = Cast<AUsable>(StartingUsables[i]->GetDefaultObject());
			if (Usable)
			{
				UE_LOG(LogTemp, Warning, TEXT("UsablePointer"));
				Usable->BuildUsable(*Usable->StartingName);
				UsablesInventory.Add(Usable);
			}
		}
		
	}
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetCharacterMovement()->MaxWalkSpeed = bMobilityModifier ? MaxWalkSpeed * MobilityModifier : MaxWalkSpeed;

	if (bLockedOn && TargetEnemy)
	{
		float SqrDistance = FVector::DistSquared(GetActorLocation(), TargetEnemy->GetActorLocation());
		if (SqrDistance > FMath::Square(MaxLockOnDistance))
		{
			UntargetEnemy();
			SwitchMovementStyle(EMovementStyle::EMS_Free);
			SwitchCameraMovement(ECameraMovement::ECM_Free);
			return;
		}

		LockOnEnemy();
		LookAtTarget();
	}
	else if (bInventoryOpen)
	{
		LookAtInventory();
	}

	if (!bInventoryOpen)
	{
		CheckOverlappingPickups();
	}

	if (bStaminaCanRecharge)
	{
		if (Stamina < MaxStamina)
		{
			Stamina += StaminaRechargeRate * DeltaTime;
			if (Stamina > MaxStamina) Stamina = MaxStamina;
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMB);
	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBPressed);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBReleased);
	PlayerInputComponent->BindAction("MMB", IE_Pressed, this, &AMainCharacter::MMB);
	PlayerInputComponent->BindAction("Space", IE_Pressed, this, &AMainCharacter::Dodge);
	PlayerInputComponent->BindAction("E", IE_Pressed, this, &AMainCharacter::E);
	PlayerInputComponent->BindAction("Q", IE_Pressed, this, &AMainCharacter::Q);
	PlayerInputComponent->BindAction("1", IE_Pressed, this, &AMainCharacter::OnePressed);
	PlayerInputComponent->BindAction("2", IE_Pressed, this, &AMainCharacter::TwoPressed);
	PlayerInputComponent->BindAction("3", IE_Pressed, this, &AMainCharacter::ThreePressed);
	PlayerInputComponent->BindAction("4", IE_Pressed, this, &AMainCharacter::FourPressed);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AMainCharacter::MoveForward(float Value)
{
	InputVector.X = Value;

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(Direction, Value);
}

void AMainCharacter::MoveRight(float Value)
{
	InputVector.Y = Value;

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Direction, Value);
}

void AMainCharacter::LMB()
{
	if (bAttacking || bBlocking || bDodging || bStunned || bInventoryOpen || Stamina <= 0.f || RightHandEquipment == nullptr) return;

	bAttacking = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		FString SectionName = FString::Printf(TEXT("Attack%d"), ComboCount);
		AnimInstance->Montage_Play(AttackMontage);
		if (bCanCombo)
		{
			AnimInstance->Montage_JumpToSection(*SectionName, AttackMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(*SectionName, AttackMontage);
		}

		UseStamina(RightHandEquipment->StaminaCost, RightHandEquipment->StaminRechargeDelay);
		bStaminaCanRecharge = false;
	}
}

void AMainCharacter::RMBPressed()
{
	if (!bAttacking && !bDodging && !bInventoryOpen && RightHandEquipment)
	{
		bBlocking = true;
		if (RightHandEquipment)
		{
			RightHandEquipment->ActivateGuardCollision();
		}
	}
}

void AMainCharacter::RMBReleased()
{
	bBlocking = false;
	if (RightHandEquipment)
	{
		RightHandEquipment->DeactivateGuardCollision();
	}
}

void AMainCharacter::MMB()
{
	if (bInventoryOpen) return;

	bool LockedOn = FindBestTargetEnemy();
	if (LockedOn)
	{

	}
	else
	{
		UntargetEnemy();
		SwitchCameraMovement(ECameraMovement::ECM_Free);
	}

	SwitchMovementStyle(EMovementStyle::EMS_Free);
}

void AMainCharacter::E()
{
	if (bCanPickup && !bInventoryOpen)
	{
		if (CurrentPickup->BPPickupItem)
		{
			TSubclassOf<AWeapon> PickupWeapon = CurrentPickup->BPPickupItem;
			if (PickupWeapon)
			{
				EquipmentInventory.Add(PickupWeapon);
				if (EquipmentInventory.Num() == 1) EquippedWeaponIndex = 0;

				CurrentPickup->Destroy();
				DynamicMulticastSetImageAndCount.Broadcast();
			}

			TSubclassOf<AResource> PickupResource = CurrentPickup->BPPickupItem;
			if (PickupResource)
			{
				AResource* Resource = Cast<AResource>(PickupResource->GetDefaultObject());
				if (Resource)
				{
					Resource->BuildResource(CurrentPickup->ItemName);
					FString Name = Resource->ResourceName;
					Resource->SetActorLabel(Name);
					ResourceInventory.Add(Resource);
					ResourceInventory.Sort();

					CurrentPickup->Destroy();
					DynamicMulticastSetImageAndCount.Broadcast();
				}
			}

			if (OverlappingPickups.Num() == 0)
			{
				MainPlayerController->RemovePickupPrompt();
			}
		}
	}
}

void AMainCharacter::Q()
{
	if (MainPlayerController)
	{
		bBlocking = false;

		bInventoryOpen = !bInventoryOpen;
		MainPlayerController->ToggleInventory(bInventoryOpen);
		if (bInventoryOpen)
		{
			MainPlayerController->RemovePickupPrompt();
			SwitchMovementStyle(EMovementStyle::EMS_Free);
			SwitchCameraMovement(ECameraMovement::EMC_Inventory);

			CameraFixedRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
			SpringArm->SetWorldRotation(CameraFixedRotation);

			DynamicMulticastOpenCharacterMenu.Broadcast();
		}
		else
		{
			SwitchCameraMovement(ECameraMovement::ECM_Free);
			DynamicMulticastResetInventoryHUD.Broadcast();
		}

		if (bLockedOn)
		{
			UntargetEnemy();
		}
	}
}

void AMainCharacter::OnePressed()
{
	if (GearSlotOneInventory.Num() > 0 && !bInventoryOpen && !bUsing)
	{
		GetGear(0);
	}
}

void AMainCharacter::TwoPressed()
{
	if (GearSlotTwoInventory.Num() > 0 && !bInventoryOpen && !bUsing)
	{
		GetGear(1);
	}
}

void AMainCharacter::ThreePressed()
{
	if (GearSlotThreeInventory.Num() > 0 && !bInventoryOpen && !bUsing)
	{
		GetGear(2);
	}
}

void AMainCharacter::FourPressed()
{
	if (GearSlotFourInventory.Num() > 0 && !bInventoryOpen && !bUsing)
	{
		GetGear(3);
	}
}

void AMainCharacter::SetHealth(float Amount)
{
	if (Amount > MaxHealth) Health = MaxHealth;
	else if (Amount < 0) Health = 0;
	else Health = Amount;
}

void AMainCharacter::EnemyDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*if (OtherActor && OtherActor != this)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Detected: %s"), *OtherActor->GetName());
		}
	}*/
}

void AMainCharacter::EnemyDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	/*if (OtherActor && OtherActor != this)
	{
		APawn* Pawn = Cast<APawn>(OtherActor);
		if (Pawn)
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy Lost: %s"), *OtherActor->GetName());
		}
	}*/
}

void AMainCharacter::CheckOverlappingPickups()
{
	if (OverlappingPickups.Num() > 0)
	{
		float BestPickup = 100000.f;
		for (APickup* Pickup : OverlappingPickups)
		{
			float SqrDistance = FVector::DistSquared(GetActorLocation(), Pickup->GetActorLocation());
			if (SqrDistance < BestPickup)
			{
				CurrentPickup = Pickup;
				BestPickup = SqrDistance;
			}
		}

		if (CurrentPickup)
		{
			MainPlayerController->DisplayPickupPrompt(CurrentPickup->GetPickupPrompt());
		}
	}
	else if (OverlappingPickups.Num() == 1)
	{
		CurrentPickup = OverlappingPickups[0];
		if (CurrentPickup)
		{
			MainPlayerController->DisplayPickupPrompt(CurrentPickup->GetPickupPrompt());
		}
	}
}

void AMainCharacter::SetPickups(APickup* Pickup, bool Overlapped)
{
	if (Overlapped)
	{
		PickupsCount++;
		bCanPickup = PickupsCount > 0;
		OverlappingPickups.Add(Pickup);
	}
	else
	{
		PickupsCount--;
		bCanPickup = PickupsCount > 0;
		OverlappingPickups.Remove(Pickup);

		if (PickupsCount == 0)
		{
			MainPlayerController->RemovePickupPrompt();
		}
	}
}

bool AMainCharacter::FindBestTargetEnemy()
{
	if (bLockedOn) return false;

	float BestEnemyValue = 100000.f;

	TArray<AActor*> OverlappedEnemies;
	EnemyDetectionSphere->GetOverlappingActors(OverlappedEnemies, AEnemy::StaticClass());
	if (OverlappedEnemies.Num() > 0)
	{
		for (AActor* Enemy : OverlappedEnemies)
		{
			if (Enemy == this) continue;

			//Can Camera and Character can see Enemy
			FHitResult HitResult;
			FVector CameraStart = Camera->GetComponentLocation();
			FVector End = Enemy->GetActorLocation();
			GetWorld()->LineTraceSingleByChannel(HitResult, CameraStart, End, ECollisionChannel::ECC_Visibility);
			if (HitResult.bBlockingHit)
			{
				continue;
			}
			else
			{
				FVector CharacterStart = GetActorLocation();
				GetWorld()->LineTraceSingleByChannel(HitResult, CharacterStart, End, ECollisionChannel::ECC_Visibility);
				if(HitResult.bBlockingHit) continue;
			}

			//Is Enemy in range
			FVector EnemyVector = Enemy->GetActorLocation() - GetActorLocation();
			float Distance = EnemyVector.Size();
			if (Distance > MaxLockOnDistance) continue;
			else
			{
				Distance *= 0.5f;
			}

			FVector2D ScreenSize;
			if (GEngine && GEngine->GameViewport)
			{
				GEngine->GameViewport->GetViewportSize(ScreenSize);
			}

			FVector2D ScreenCenter(ScreenSize.X * 0.5f, ScreenSize.Y * 0.5f);
			FVector2D EnemyScreenPosition;
			bool bEnemyScreenToWorld = UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), Enemy->GetActorLocation(), EnemyScreenPosition);

			FVector2D EnemyScreenVector = EnemyScreenPosition - ScreenCenter;
			float ScreenCenterDistance = EnemyScreenVector.Size();

			if (ScreenCenterDistance + Distance < BestEnemyValue)
			{
				BestEnemyValue = ScreenCenterDistance + Distance;
				TargetEnemy = Cast<AEnemy>(Enemy);
			}
		}

		if (TargetEnemy != nullptr)
		{
			bLockedOn = true;
			TargetEnemy->ShowHealthBar(true);
			return true;
		}
	}

	return false;
}

void AMainCharacter::LockOnEnemy()
{
	if (MainPlayerController && TargetEnemy)
	{
		if (TargetEnemy->LockOnPosition)
		{
			MainPlayerController->DisplayLockOn(TargetEnemy->LockOnPosition->GetComponentLocation());
		}
	}
}

void AMainCharacter::UntargetEnemy()
{
	if (MainPlayerController && TargetEnemy)
	{
		TargetEnemy->HideHealthBar();
		TargetEnemy = nullptr;
		MainPlayerController->RemoveLockOn();
		bLockedOn = false;
	}
}

void AMainCharacter::ResetState()
{
	bAttacking = bDodging = bStunned = false;
}

void AMainCharacter::SwitchMovementStyle(EMovementStyle MovementStyle)
{
	switch (MovementStyle)
	{
	case EMovementStyle::EMS_Free:
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;

		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
		break;
	case EMovementStyle::EMS_LockedOn:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		break;
	default:
		break;
	}
}

void AMainCharacter::SwitchCameraMovement(ECameraMovement CameraMovement)
{
	switch (CameraMovement)
	{
	case ECameraMovement::ECM_Free:
		SpringArm->SetWorldLocation(RootComponent->GetComponentLocation() + FVector(0.f, 0.f, 60.f));
		SpringArm->TargetArmLength = SpringArmDefaultLength;
		SpringArm->bUsePawnControlRotation = true;
		break;
	case ECameraMovement::EMC_LockedOn:
		break;
	case ECameraMovement::EMC_Inventory:
		SpringArm->TargetArmLength = 250.f;
		SpringArm->bUsePawnControlRotation = false;
		break;
	default:
		break;
	}
}

void AMainCharacter::LookAtTarget()
{
	//Rotate Actor to face Enemy
	if (bFaceTarget)
	{
		FVector LookVector = TargetEnemy->GetActorLocation() - GetActorLocation();
		LookVector.Z = 0.f;
		FRotator LookRotation = LookVector.Rotation();

		SetActorRotation(LookRotation);
	}

	//Set SprinArm location between Player and Enemy
	FVector CombatCenter = GetActorLocation() + (TargetEnemy->LockOnPosition->GetComponentLocation() - GetActorLocation()) * 0.5;
	SpringArm->SetWorldLocation(CombatCenter);

	//Adjust SpringArm length based on distance 
	float CombatDistance = FVector::Distance(GetActorLocation(), TargetEnemy->GetActorLocation());
	CombatDistance = FMath::Clamp(CombatDistance, 500.f, MaxLockOnDistance);
	SpringArm->TargetArmLength = CombatDistance;
}

void AMainCharacter::LookAtInventory()
{
	//FRotator CameraRotation{ 0.f, GetControlRotation().Yaw, 0.f };
	SpringArm->SetWorldRotation(CameraFixedRotation);

	FVector LeftDirection = FVector::CrossProduct(Camera->GetForwardVector(), FVector::UpVector) * 100.f;
	SpringArm->SetWorldLocation(GetActorLocation() + LeftDirection);
}

void AMainCharacter::EquipWeaponR(int32 Index)
{
	if (EquipmentInventory.Num() > Index)
	{
		if (EquipmentInventory[Index])
		{
			Super::EquipRightHand(EquipmentInventory[Index]);
		}
	}
	else
	{
		Super::EquipRightHand(nullptr);
	}
}

void AMainCharacter::EquipWeaponL(int32 Index)
{
	if (EquipmentInventory.Num() > Index)
	{
		if (EquipmentInventory[Index])
		{
			if (RightHandEquipment)
			{

			}
			Super::EquipLeftHand(EquipmentInventory[Index]);
		}
	}
	else
	{
		Super::EquipLeftHand(nullptr);
	}
}

bool AMainCharacter::RemoveAndSetIngredient(int32 ResourceStackIndex, int32 ResourceSelectIndex, UTexture2D*& ResourceImage)
{
	if (ResourceSelectIndex == 0)
	{
		if (SetIngredientOne)
		{
			ResourceInventory.Add(SetIngredientOne);
			SetIngredientOne = nullptr;
		}
	}
	else if (ResourceSelectIndex == 1)
	{
		if (SetIngredientTwo)
		{
			ResourceInventory.Add(SetIngredientTwo);
			SetIngredientTwo = nullptr;
		}
	}

	int32 Index = 0;
	int32 ResourceIndex = 0;

	if (ResourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			ResourceIndex = i;
			if (i == 0) PreviousResource = ResourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = ResourceInventory[i];

			if (CurrentResource->ResourceName != PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					ResourceImage = CurrentResource->ResourceImage;
					if (ResourceSelectIndex == 0)
					{
						/*if (SetIngredientsOne.Num() > 0)
						{
							if (SetIngredientsOne[0]->ResourceName != CurrentResource->ResourceName)
							{
								while (SetIngredientsOne.Num() > 0)
								{
									ResourceInventory.Add(SetIngredientsOne[0]);
									SetIngredientsOne.RemoveAt(0);
								}
								SetIngredientsOne.Add(CurrentResource);
								ResourceInventory.Sort();
							}
							else
							{
								SetIngredientsOne.Add(CurrentResource);
							}
						}*/

						SetIngredientOne = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
					else
					{
						SetIngredientTwo = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
				}

				Index++;
				if (Index == ResourceStackIndex)
				{
					ResourceImage = CurrentResource->ResourceImage;
					if (ResourceSelectIndex == 0)
					{
						SetIngredientOne = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
					else
					{
						SetIngredientTwo = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
				}
			}
			else if (CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					ResourceImage = CurrentResource->ResourceImage;
					if (ResourceSelectIndex == 0)
					{
						SetIngredientOne = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
					else
					{

						SetIngredientTwo = CurrentResource;
						ResourceInventory.RemoveAt(i);
						ResourceInventory.Sort();
						return true;
					}
				}
			}
		}
	}

	return false;
}

void AMainCharacter::ResetCraftingIngredients(bool ResetFirst, bool ResetSecond)
{
	if (SetIngredientOne && ResetFirst)
	{
		ResourceInventory.Add(SetIngredientOne);
		SetIngredientOne = nullptr;
		UsedIngredientOneName = "";
	}
	if (SetIngredientTwo && ResetSecond)
	{
		ResourceInventory.Add(SetIngredientTwo);
		SetIngredientTwo = nullptr;
		UsedIngredientTwoName = "";
	}

	ResourceInventory.Sort();
}

void AMainCharacter::GetResource(int32 ResourceStackIndex, int32 InUseIngredientIndex, int32& ResourceInventoryIndex, UTexture2D*& ResourceImage, bool& bHasResource)
{
	int32 Index = 0;

	if(ResourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			ResourceInventoryIndex = i;
			if (i == 0) PreviousResource = ResourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = ResourceInventory[i];

			if (CurrentResource->ResourceName != PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					if (i - 1 != InUseIngredientIndex)
					{
						ResourceImage = PreviousResource->ResourceImage;
						ResourceInventoryIndex = i - 1;
						bHasResource = true;
						return;
					}
					/*else if (i != InUseIngredientIndex)
					{
						UE_LOG(LogTemp, Warning, TEXT("new previous"));
						ResourceImage = PreviousResource->ResourceImage;
						ResourceInventoryIndex = i - 1;
						bHasResource = true;
						return;
					}*/
				}
				Index++;	
			}
			else if (CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					if (i != InUseIngredientIndex)
					{
						ResourceImage = PreviousResource->ResourceImage;
						ResourceInventoryIndex = i;
						bHasResource = true;
						return;
					}
					else if (i - 1 != InUseIngredientIndex && ResourceInventory[i]->ResourceName == CurrentResource->ResourceName)
					{
						ResourceImage = PreviousResource->ResourceImage;
						ResourceInventoryIndex = i - 1;
						bHasResource = true;
						return;
					}
				}
			}
		}

		if (Index == ResourceStackIndex && ResourceInventoryIndex != InUseIngredientIndex)
		{
			ResourceImage = CurrentResource->ResourceImage;
			bHasResource = true;
			return;
		}
	}

	bHasResource = false;
}

void AMainCharacter::GetResourceImage(int32 ResourceStackIndex, UTexture2D*& ResourceImage)
{
	int32 Index = 0;

	if (ResourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			if (i == 0) PreviousResource = ResourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = ResourceInventory[i];

			if (CurrentResource->ResourceName != PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					ResourceImage = PreviousResource->ResourceImage;
					return;
				}
				Index++;
			}
			else if (CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					ResourceImage = CurrentResource->ResourceImage;
					return;
				}
			}
		}

		if (Index == ResourceStackIndex)
		{
			ResourceImage = CurrentResource->ResourceImage;
			return;
		}
	}
}

void AMainCharacter::GetResourceCount(int32 ResourceStackIndex, int32& ResourceCount)
{
	int32 Index = 0;
	int32 ItemCount = 0;

	if (ResourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			if (i == 0) PreviousResource = ResourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = ResourceInventory[i];

			if (CurrentResource->ResourceName != PreviousResource->ResourceName)
			{
				if (Index == ResourceStackIndex)
				{
					ResourceCount = ItemCount;
					return;
				}
				Index++;
				ItemCount = 1;
			}
			else if (CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				ItemCount++;
			}
		}

		if (Index == ResourceStackIndex)
		{
			ResourceCount = ItemCount;
			return;
		}

		ItemCount = 0;
	}
}

UTexture2D* AMainCharacter::CheckCanCraft()
{
	UTexture2D* Image = nullptr;

	if (SetIngredientOne)
	{
		if (SetIngredientTwo)
		{
			FName Craftable = *SetIngredientOne->CheckCanCombine(SetIngredientTwo);
			if (Craftable != "")
			{
				AUsable* Usable = NewObject<AUsable>();
				if (Usable)
				{
					Usable->BuildUsable(Craftable);
					if (Usable->UsableImage)
					{
						Image = Usable->UsableImage;
						return Image;
					}
				}
			}
		}
	}

	return Image;
}

bool AMainCharacter::AddUsable()
{
	if (SetIngredientOne)
	{
		if (SetIngredientTwo)
		{
			FName Craftable = *SetIngredientOne->CheckCanCombine(SetIngredientTwo);
			if (Craftable != "")
			{
				AUsable* Usable = NewObject<AUsable>();
				if (Usable)
				{
					Usable->BuildUsable(Craftable);
					if (Usable->UsableImage)
					{
						Usable->BuildUsable(Craftable);
						FString Name = Usable->UsableName;
						Usable->SetActorLabel(Name);
						UsablesInventory.Add(Usable);
						//QuickSortUsables(UsablesInventory, 0, UsablesInventory.Num() - 1);

						UsedIngredientOneName = *SetIngredientOne->ResourceName;
						UsedIngredientTwoName = *SetIngredientTwo->ResourceName;

						SetIngredientOne = nullptr;
						SetIngredientTwo = nullptr;

						return true;
					}
				}
			}
		}
	}

	UsedIngredientOneName = "";
	UsedIngredientTwoName = "";
	return false;
}

bool AMainCharacter::CheckCanCraftMore()
{
	bool bHasMoreIngredientOne = false;
	bool bHasMoreIngredientTwo = false;

	if (UsedIngredientOneName != "")
	{
		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			if (*ResourceInventory[i]->ResourceName == UsedIngredientOneName)
			{
				bHasMoreIngredientOne = true;
				break;
			}
		}
	}

	if (UsedIngredientTwoName != "")
	{
		for (int i = 0; i < ResourceInventory.Num(); i++)
		{
			if (*ResourceInventory[i]->ResourceName == UsedIngredientTwoName)
			{
				bHasMoreIngredientTwo = true;
				break;
			}
		}
	}

	return bHasMoreIngredientOne && bHasMoreIngredientTwo;
}

void AMainCharacter::ResetIngredients()
{
	if (SetIngredientOne)
	{
		ResourceInventory.Add(SetIngredientOne);
		SetIngredientOne = nullptr;
	}
	if (SetIngredientTwo)
	{
		ResourceInventory.Add(SetIngredientTwo);
		SetIngredientTwo = nullptr;
	}
	ResourceInventory.Sort();

	UsedIngredientOneName = "";
	UsedIngredientTwoName = "";
}

UTexture2D* AMainCharacter::GetEquipmentImage(int32 Index)
{
	UTexture2D* Image = nullptr;
	int ItemIndex = 1;

	if (EquipmentInventory.Num() > 0)
	{
		for (int i = 0; i < EquipmentInventory.Num(); i++)
		{
			if (i == Index)
			{
				AWeapon* Weapon = Cast<AWeapon>(EquipmentInventory[i]->GetDefaultObject());
				if (Weapon)
				{
					if (Weapon->ItemImage)
					{
						return Weapon->ItemImage;
					}
				}
			}
		}
	}

	return Image;
}

UTexture2D* AMainCharacter::GetGearImage(int32 Index, int32& ItemCount)
{
	UTexture2D* Image = nullptr;
	int ItemIndex = 0;
	ItemCount = 0;

	if (Index == -1) return nullptr;

	if (UsablesInventory.Num() > 0)
	{
		AUsable* CurrentUsable = nullptr;
		AUsable* PreviousVisible = nullptr;

		for (int i = 0; i < UsablesInventory.Num(); i++)
		{
			if (i == 0) PreviousVisible = UsablesInventory[0];
			else PreviousVisible = CurrentUsable;
			CurrentUsable = UsablesInventory[i];

			if (CurrentUsable->UsableName != PreviousVisible->UsableName)
			{
				if (ItemIndex == Index)
				{
					return Image;
				}
				ItemIndex++;
				ItemCount = 1;
			}
			else if (CurrentUsable->UsableName == PreviousVisible->UsableName)
			{
				ItemCount++;
			}

			if(ItemIndex == Index)
			{
				Image = CurrentUsable->UsableImage;
			}
			else
			{
				ItemCount = 0;
				Image = nullptr;
			}
		}
	}

	return Image;
}

void AMainCharacter::GetGear(int32 Index)
{
	switch (Index)
	{
	case 0:
		DesiredGearToUse = GearSlotOneInventory[0];
		DesiredGearSlot = 0;
		break;
	case 1:
		DesiredGearToUse = GearSlotTwoInventory[0];
		DesiredGearSlot = 1;
		break;
	case 2:
		DesiredGearToUse = GearSlotThreeInventory[0];
		DesiredGearSlot = 2;
		break;
	case 3:
		DesiredGearToUse = GearSlotFourInventory[0];
		DesiredGearSlot = 3;
		break;
	}

	if (DesiredGearToUse)
	{
		switch (DesiredGearToUse->StatusEffect)
		{
		case EStatusEffect::ESE_None:
			return;
		case EStatusEffect::ESE_Attack:
			PlayUseMontage("CrushPotion");
			return;
		case EStatusEffect::ESE_Heal:
			PlayUseMontage("DrinkPotion");
			return;
		case EStatusEffect::ESE_Defense:
			PlayUseMontage("CrushPotion");
			return;
		case EStatusEffect::ESE_Mobility:
			PlayUseMontage("CrushPotion");
			break;
		default:
			return;
		}
	}
}

void AMainCharacter::UseDesiredGear()
{
	if (DesiredGearToUse)
	{
		switch (DesiredGearSlot)
		{
		case 0:
			GearSlotOneInventory.RemoveAt(0);
			break;
		case 1:
			GearSlotTwoInventory.RemoveAt(0);
			break;
		case 2:
			GearSlotThreeInventory.RemoveAt(0);
			break;
		case 3:
			GearSlotFourInventory.RemoveAt(0);
			break;
		}

		switch (DesiredGearToUse->StatusEffect)
		{
		case EStatusEffect::ESE_None:
			return;
		case EStatusEffect::ESE_Attack:
			SetAttackModifier(DesiredGearToUse->StatusEffectTime, DesiredGearToUse->AttackModifier);
			DynamicMulticastSetImageAndCount.Broadcast();
			DynamicMulticastSetAttackTimer.Broadcast(true);
			return;
		case EStatusEffect::ESE_Heal:
			Heal(DesiredGearToUse->HealAmount);
			DynamicMulticastSetImageAndCount.Broadcast();
			return;
		case EStatusEffect::ESE_Defense:
			SetDefenseModifier(DesiredGearToUse->StatusEffectTime, DesiredGearToUse->DefenseModifier);
			DynamicMulticastSetImageAndCount.Broadcast();
			DynamicMulticastSetDefenseTimer.Broadcast(true);
			return;
		case EStatusEffect::ESE_Mobility:
			SetMobilityModifier(DesiredGearToUse->StatusEffectTime, DesiredGearToUse->MobilityModifier);
			DynamicMulticastSetImageAndCount.Broadcast();
			DynamicMulticastSetMobilityTimer.Broadcast(true);
			break;
		default:
			return;
		}

		DesiredGearToUse = nullptr;
		DesiredGearSlot = -1;
	}
}

void AMainCharacter::SetAttackModifier(int32 Time, float Modifier)
{
	bAttackModifier = true;
	AttackModifier = Modifier;
	GetWorldTimerManager().SetTimer(AttackModifierTimer, this, &AMainCharacter::ResetAttackModifier, Time);
}

void AMainCharacter::ResetAttackModifier()
{
	bAttackModifier = false;
	AttackModifier = 1;
	DynamicMulticastSetAttackTimer.Broadcast(false);
}

float AMainCharacter::GetAttackTimeRemaining()
{
	if (bAttackModifier)
	{
		return GetWorldTimerManager().GetTimerRemaining(AttackModifierTimer);
	}

	return 0;
}

void AMainCharacter::SetDefenseModifier(int32 Time, float Modifier)
{
	bDefenseModifier = true;
	DefenseModifier = Modifier;
	GetWorldTimerManager().SetTimer(DefenseModifierTimer, this, &AMainCharacter::ResetAttackModifier, Time);
}

void AMainCharacter::ResetDefenseModifier()
{
	bDefenseModifier = false;
	DefenseModifier = 1;
	DynamicMulticastSetMobilityTimer.Broadcast(false);
}

float AMainCharacter::GetDefenseTimeRemaining()
{
	if (bDefenseModifier)
	{
		return GetWorldTimerManager().GetTimerRemaining(DefenseModifierTimer);
	}

	return 0;
}

void AMainCharacter::SetMobilityModifier(int32 Time, float Modifier)
{
	bMobilityModifier = true;
	MobilityModifier = Modifier;
	GetWorldTimerManager().SetTimer(MobilityModifierTimer, this, &AMainCharacter::ResetMobilityModifier, Time);
	CustomTimeDilation = Modifier;
}

void AMainCharacter::ResetMobilityModifier()
{
	bMobilityModifier = false;
	MobilityModifier = 1;
	CustomTimeDilation = MobilityModifier;
	DynamicMulticastSetMobilityTimer.Broadcast(false);
}

float AMainCharacter::GetMobilityTimeRemaining()
{
	if (bMobilityModifier)
	{
		return GetWorldTimerManager().GetTimerRemaining(MobilityModifierTimer);
	}

	return 0;
}

void AMainCharacter::Heal(float Amount)
{
	Health += Amount;
	if (Health > MaxHealth) Health = MaxHealth;
}

void AMainCharacter::PlayUseMontage(FName MontageSection)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		bUsing = true;
		AnimInstance->Montage_Play(UseMontage);
		AnimInstance->Montage_JumpToSection(MontageSection);
	}
}

void AMainCharacter::SetGearUseIndex(int32 GearBoxIndex, int32 DesiredInventoryIndex)
{
	int32 SwapIndex = -1;
	int32 SwapValue = -1;

	for (int i = 0; i < CurrentGearIndexes.Num(); i++)
	{
		if (i == GearBoxIndex)
		{
			SwapValue = CurrentGearIndexes[i];
			CurrentGearIndexes[i] = DesiredInventoryIndex;
		}

		if (i != GearBoxIndex && CurrentGearIndexes[i] == DesiredInventoryIndex)
		{
			SwapIndex = i;
		}
	}

	if (SwapIndex >= 0)
	{
		CurrentGearIndexes[SwapIndex] = SwapValue;
	}
}

void AMainCharacter::GetGearStack(int32 StackIndex, int32 SlotIndex)
{
	int32 Index = 0;
	int32 StartIndex = 0;
	int32 EndIndex = 0;

	if (UsablesInventory.Num() > 0)
	{
		AUsable* CurrentUsable = nullptr;
		AUsable* PreviousUsable = nullptr;

		for (int i = 0; i < UsablesInventory.Num(); i++)
		{
			if (i == 0) PreviousUsable = UsablesInventory[0];
			else PreviousUsable = CurrentUsable;
			CurrentUsable = UsablesInventory[i];

			if (CurrentUsable->UsableName == PreviousUsable->UsableName)
			{
				if (Index == StackIndex)
				{
					if (i == 0) StartIndex = i;
				}
			}
			if (CurrentUsable->UsableName != PreviousUsable->UsableName)
			{
				Index++;
				if (Index == StackIndex)
				{
					StartIndex = i;
				}
			}
		}

		for (int i = StartIndex, j = 0; i < UsablesInventory.Num(); j++)
		{
			if (j == 0) PreviousUsable = UsablesInventory[i];
			else PreviousUsable = CurrentUsable;
			CurrentUsable = UsablesInventory[i];

			if (CurrentUsable->UsableName == PreviousUsable->UsableName)
			{
				AddToGearSlot(UsablesInventory[StartIndex], SlotIndex);
				UsablesInventory.RemoveAt(StartIndex);
			}
			else
			{
				return;
			}
		}
	}
}

void AMainCharacter::RemoveGearStack(int32 SlotIndex)
{
	switch (SlotIndex)
	{
	case 0:
		while (GearSlotOneInventory.Num() > 0)
		{
			UsablesInventory.Add(GearSlotOneInventory[0]);
			GearSlotOneInventory.RemoveAt(0);
		}
		break;
	case 1:
		while (GearSlotTwoInventory.Num() > 0)
		{
			UsablesInventory.Add(GearSlotTwoInventory[0]);
			GearSlotTwoInventory.RemoveAt(0);
		}
		break;
	case 2:
		while (GearSlotThreeInventory.Num() > 0)
		{
			UsablesInventory.Add(GearSlotThreeInventory[0]);
			GearSlotThreeInventory.RemoveAt(0);
		}
		break;
	case 3:
		while (GearSlotFourInventory.Num() > 0)
		{
			UsablesInventory.Add(GearSlotFourInventory[0]);
			GearSlotFourInventory.RemoveAt(0);
		}
		break;
	}
}

void AMainCharacter::AddToGearSlot(AUsable* UsableToAdd, int32 SlotIndex)
{
	switch (SlotIndex)
	{
	case 0:
		if (GearSlotOneInventory.Num() > 0)
		{
			if (GearSlotOneInventory[0]->UsableName != UsableToAdd->UsableName)
			{
				while(GearSlotOneInventory.Num() > 0)
				{
					UsablesInventory.Add(GearSlotOneInventory[0]);
					GearSlotOneInventory.RemoveAt(0);
				}
			}
		}
		GearSlotOneInventory.Add(UsableToAdd);
		break;
	case 1:
		if (GearSlotTwoInventory.Num() > 0)
		{
			if (GearSlotTwoInventory[0]->UsableName != UsableToAdd->UsableName)
			{
				while (GearSlotTwoInventory.Num() > 0)
				{
					UsablesInventory.Add(GearSlotTwoInventory[0]);
					GearSlotTwoInventory.RemoveAt(0);
				}
			}
		}
		GearSlotTwoInventory.Add(UsableToAdd);
		break;
	case 2:
		if (GearSlotThreeInventory.Num() > 0)
		{
			if (GearSlotThreeInventory[0]->UsableName != UsableToAdd->UsableName)
			{
				while (GearSlotThreeInventory.Num() > 0)
				{
					UsablesInventory.Add(GearSlotThreeInventory[0]);
					GearSlotThreeInventory.RemoveAt(0);
				}
			}
		}
		GearSlotThreeInventory.Add(UsableToAdd);
		break;
	case 3:
		if (GearSlotFourInventory.Num() > 0)
		{
			if (GearSlotFourInventory[0]->UsableName != UsableToAdd->UsableName)
			{
				while (GearSlotFourInventory.Num() > 0)
				{
					UsablesInventory.Add(GearSlotFourInventory[0]);
					GearSlotFourInventory.RemoveAt(0);
				}
			}
		}
		GearSlotFourInventory.Add(UsableToAdd);
		break;
	}
}

void AMainCharacter::SwapGearSlot(int32 FirstSlot, int32 SecondSlot)
{
	TArray<AUsable*> TempOne;
	TArray<AUsable*> TempTwo;

	switch (FirstSlot)
	{
	case 0:
		TempOne = GearSlotOneInventory;
		break;
	case 1:
		TempOne = GearSlotTwoInventory;
		break;
	case 2:
		TempOne = GearSlotThreeInventory;
		break;
	case 3:
		TempOne = GearSlotFourInventory;
		break;
	}

	switch (SecondSlot)
	{
	case 0:
		TempTwo = GearSlotOneInventory;
		GearSlotOneInventory = TempOne;
		break;
	case 1:
		TempTwo = GearSlotTwoInventory;
		GearSlotTwoInventory = TempOne;
		break;
	case 2:
		TempTwo = GearSlotThreeInventory;
		GearSlotThreeInventory = TempOne;
		break;
	case 3:
		TempTwo = GearSlotFourInventory;
		GearSlotFourInventory = TempOne;
		break;
	}

	switch (FirstSlot)
	{
	case 0:
		GearSlotOneInventory = TempTwo;
		break;
	case 1:
		GearSlotTwoInventory = TempTwo;
		break;
	case 2:
		GearSlotThreeInventory = TempTwo;
		break;
	case 3:
		GearSlotFourInventory = TempTwo;
		break;
	}
}

void AMainCharacter::GetGearSlotImageAndCount(int32 SlotIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear)
{
	switch (SlotIndex)
	{
	case 0:
		if (GearSlotOneInventory.Num() > 0)
		{
			OutImage = GearSlotOneInventory[0]->UsableImage;
			Count = GearSlotOneInventory.Num();
			HasGear = true;
		}
		else
		{
			HasGear = false;
		}
		break;
	case 1:
		if (GearSlotTwoInventory.Num() > 0)
		{
			OutImage = GearSlotTwoInventory[0]->UsableImage;
			Count = GearSlotTwoInventory.Num();
			HasGear = true;
		}
		else
		{
			HasGear = false;
		}
		break;
	case 2:
		if (GearSlotThreeInventory.Num() > 0)
		{
			OutImage = GearSlotThreeInventory[0]->UsableImage;
			Count = GearSlotThreeInventory.Num();
			HasGear = true;
		}
		else
		{
			HasGear = false;
		}
		break;
	case 3:
		if (GearSlotFourInventory.Num() > 0)
		{
			OutImage = GearSlotFourInventory[0]->UsableImage;
			Count = GearSlotFourInventory.Num();
			HasGear = true;
		}
		else
		{
			HasGear = false;
		}
		break;
	}
}

void AMainCharacter::GetGearInventoryStackImageAndCount(int32 StackIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear)
{
	int32 Index = 0;
	Count = 0;
	HasGear = false;

	if (UsablesInventory.Num() > 0)
	{
		AUsable* PreviousUsable = nullptr;
		AUsable* CurrentUsable = nullptr;

		for (int i = 0; i < UsablesInventory.Num(); i++)
		{
			if (i == 0) PreviousUsable = UsablesInventory[0];
			else PreviousUsable = CurrentUsable;
			CurrentUsable = UsablesInventory[i];

			if (CurrentUsable->UsableName == PreviousUsable->UsableName)
			{
				Count++;
			}
			else if (CurrentUsable->UsableName != PreviousUsable->UsableName)
			{
				if (Index == StackIndex)
				{
					OutImage = PreviousUsable->UsableImage;
					HasGear = true;
					return;
				}
				Index++;
				Count = 1;
			}
		}

		if (Index == StackIndex)
		{
			OutImage = CurrentUsable->UsableImage;
			HasGear = true;
			return;
		}
	}
}

void AMainCharacter::QuickSortUsables(TArray<AUsable*> Inventory, int32 Low, int32 High)
{
	if (Low < High)
	{
		int32 pi = Partition(Inventory, Low, High);

		QuickSortUsables(Inventory, Low, pi - 1);
		QuickSortUsables(Inventory, pi + 1, High);
	}
}

int32 AMainCharacter::Partition(TArray<AUsable*> Inventory, int32 Low, int32 High)
{
	AUsable* Pivot = Inventory[High];

	int32 i = Low - 1;

	for (int32 j = Low; j <= High - 1; j++)
	{
		if (Inventory[j]->UsableID < Pivot->UsableID)
		{
			i++;
			TestSwap(i, j);
		}
	}

	TestSwap(i + 1, High);
	return i + 1;
}

void AMainCharacter::TestSwap(int32 i, int32 j)
{
	AUsable* Temp = UsablesInventory[i];
	UsablesInventory[i] = UsablesInventory[j];
	UsablesInventory[j] = Temp;
}

void AMainCharacter::QuickSortDuplicates(TArray<AUsable*> Inventory, int Left, int Right)
{
	if (Right <= Left) return;

	int i = 0, j = 0;

	PartitionDuplicates(Inventory, Left, Right, i, j);

	QuickSortDuplicates(Inventory, Left, j);
	QuickSortDuplicates(Inventory, i, Right);
}

void AMainCharacter::PartitionDuplicates(TArray<AUsable*> Arr, int Left, int Right, int i, int j)
{
	i = Left - 1;
	j = Right;
	int p = Left - 1, q = Right;
	int v = Arr[Right]->UsableID;

	while (true)
	{
		while (Arr[++i]->UsableID < v);

		while (v < Arr[--j]->UsableID)
			if (j == Left)
				break;

		if (i >= j)
			break;

		Swap(i, j);

		if (Arr[i]->UsableID == v)
		{
			p++;
			Swap(p, i);
		}

		if (Arr[j]->UsableID == v)
		{
			q--;
			Swap(j, q);
		}
	}

	Swap(i, Right);

	j = i - 1;
	for (int k = Left; k < p; k++, j--)
		Swap(k, j);

	i = i + 1;
	for (int k = Right - 1; k > q; k--, i++)
		Swap(i, k);
}

void AMainCharacter::Swap(int32 Left, int32 Right)
{
	AUsable* Temp = UsablesInventory[Left];
	UsablesInventory[Left] = UsablesInventory[Right];
	UsablesInventory[Right] = Temp;
}

void AMainCharacter::Dodge()
{
	if (bDodging || bAttacking || bBlocking || bInventoryOpen || Stamina <= 0) return;

	bDodging = true;
	bStaminaCanRecharge = false;
	UseStamina(25.f, 1.f);
	ComboCount = 1;
	if (TargetEnemy)
	{
		if (InputVector == FVector::ZeroVector)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("Dodgeback");
			}

			return;
		}

		InputVector.Normalize();
		FVector DesiredDirection = InputVector.RotateAngleAxis(GetControlRotation().Yaw, FVector::UpVector);

		FVector RightDirection = FVector::CrossProduct(GetActorUpVector(), DesiredDirection);
		FVector ToEnemy = TargetEnemy->GetActorLocation() - GetActorLocation();
		ToEnemy.Normalize();
		FVector RelativeRight = FVector::CrossProduct(GetActorUpVector(), ToEnemy);

		float ForwardDot = FMath::RoundToFloat(FVector::DotProduct(GetActorForwardVector(), DesiredDirection));
		float RightDot = FMath::RoundToFloat(FVector::DotProduct(RelativeRight, DesiredDirection));

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && DodgeMontage)
		{
			if (ForwardDot == 1)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("DodgeForward");
			}
			else if (RightDot  == 1)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("DodgeRight");
			}
			else if (ForwardDot == -1)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("DodgeBack");
			}
			else if (RightDot == -1)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("DodgeLeft");
			}
		}
	}
	else
	{
		if (InputVector.SizeSquared() > 0)
		{
			InputVector.Normalize();
			FVector DesiredDirection = InputVector.RotateAngleAxis(GetControlRotation().Yaw, FVector::UpVector);
			SetActorRotation(DesiredDirection.Rotation());
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (InputVector.SizeSquared() == 0)
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("Dodgeback");
			}
			else
			{
				AnimInstance->Montage_Play(DodgeMontage);
				AnimInstance->Montage_JumpToSection("DodgeFront");
			}
		}
	}
}

void AMainCharacter::Block()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && GuardMontage)
	{
		AnimInstance->Montage_Play(GuardMontage);
		AnimInstance->Montage_JumpToSection("Guard_Hit");

		UseStamina(15.f, 2.f);
		bStaminaCanRecharge = false;
	}
}

void AMainCharacter::ResetDodge()
{
	bAttacking = false;
	bDodging = false;
	bCanDodge = true;
}

void AMainCharacter::UseStamina(float StaminaCost, float RechargeDelay)
{
	Stamina -= StaminaCost;
	if (Stamina < 0.f) Stamina = 0.f;
	SetStaminaRechargeTimer(RechargeDelay);
}

void AMainCharacter::SetStaminaRechargeTimer(float RechargeDelay)
{
	GetWorldTimerManager().ClearTimer(ResetStaminaRechargeTimer);
	GetWorldTimerManager().SetTimer(ResetStaminaRechargeTimer, this, &AMainCharacter::ResetStaminaRecharge, RechargeDelay);
}

void AMainCharacter::ResetStaminaRecharge()
{
	bStaminaCanRecharge = true;
}

void AMainCharacter::OpenCombo()
{
	bAttacking = false;
	bCanCombo = true;
	ComboCount++;
}

void AMainCharacter::ResetCombo()
{
	bAttacking = false;
	bCanCombo = false;
	ComboCount = 1;
}

void AMainCharacter::DeathEnd()
{
	UKismetSystemLibrary::QuitGame(this, Cast<APlayerController>(GetController()), EQuitPreference::Quit, true);
}