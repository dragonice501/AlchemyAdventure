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
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	PlayerInputComponent->BindAction("Space", IE_Pressed, this, &AMainCharacter::Dodge);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMainCharacter::LMB);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMainCharacter::ESC);
	PlayerInputComponent->BindAction("MMB", IE_Pressed, this, &AMainCharacter::MMB);
	PlayerInputComponent->BindAction("E", IE_Pressed, this, &AMainCharacter::E);
	PlayerInputComponent->BindAction("Q", IE_Pressed, this, &AMainCharacter::Q);
	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &AMainCharacter::RMBPressed);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &AMainCharacter::RMBReleased);
	PlayerInputComponent->BindAction("Space", IE_Pressed, this, &AMainCharacter::LogInventory);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AMainCharacter::LogInventory()
{
	if (ResourceInventory.Num() > 0)
	{
		for (AResource* Resource : ResourceInventory)
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Resource->ResourceName);
	}
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
	if (bAttacking || bDodging || bStunned || bInventoryOpen || Stamina <= 0.f || RightHandEquipment == nullptr) return;

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

void AMainCharacter::ESC()
{
	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
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
			}

			TSubclassOf<AResource> PickupResource = CurrentPickup->BPPickupItem;
			if (PickupResource)
			{
				AResource* Resource = Cast<AResource>(PickupResource->GetDefaultObject());
				if (Resource)
				{
					Resource->BuildResource(CurrentPickup->ItemName);
					ResourceInventory.Add(Resource);
					ResourceInventory.Sort();

					CurrentPickup->Destroy();
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
		}

		if (bLockedOn)
		{
			UntargetEnemy();
		}

		if (bInventoryOpen)
		{
			SwitchMovementStyle(EMovementStyle::EMS_Free);
			SwitchCameraMovement(ECameraMovement::EMC_Inventory);

			CameraFixedRotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
			SpringArm->SetWorldRotation(CameraFixedRotation);
		}
		else
		{
			SwitchCameraMovement(ECameraMovement::ECM_Free);
		}
	}
}

void AMainCharacter::RMBPressed()
{
	if (!bAttacking && !bDodging && !bInventoryOpen)
	{
		bBlocking = true;
	}
}

void AMainCharacter::RMBReleased()
{
	bBlocking = false;
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

UTexture2D* AMainCharacter::GetResourceImage(int32 Index, int32& ItemCount)
{
	UTexture2D* Image = nullptr;
	int ItemIndex = 0;
	ItemCount = 0;

	if (ResourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for(int i = 0; i < ResourceInventory.Num(); i++)
		{
			if (i == 0) PreviousResource = ResourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = ResourceInventory[i];

			if (CurrentResource->ResourceName != PreviousResource->ResourceName)
			{
				if (ItemIndex == Index)
				{
					return Image;
				}
				ItemIndex++;
				ItemCount = 1;
			}
			else if(CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				ItemCount++;
			}

			if (ItemIndex == Index)
			{
				Image = CurrentResource->ResourceImage;
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

bool AMainCharacter::HasResource(int32 Index, int32 InUseResourceIndex, int32& ResourceIndex)
{
	int ItemIndex = 0;

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
				ItemIndex++;
				if (ItemIndex == Index && i != InUseResourceIndex)
				{
					ResourceIndex = i;
					return true;
				}
			}
			else if (CurrentResource->ResourceName == PreviousResource->ResourceName)
			{
				if (ItemIndex == Index && i != InUseResourceIndex)
				{
					ResourceIndex = i;
					return true;
				}
			}
		}
	}

	return false;
}

UTexture2D* AMainCharacter::CheckCanCraft(int32 FirstIndex, int32 SecondIndex)
{
	UTexture2D* Image = nullptr;

	if (FirstIndex >= 0)
	{
		if (SecondIndex >= 0)
		{
			FName Craftable = *ResourceInventory[FirstIndex]->CheckCanCombine(ResourceInventory[SecondIndex]);
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

bool AMainCharacter::AddUsable(int32 FirstIngredient, int32 SecondIngredient)
{
	if (FirstIngredient >= 0)
	{
		if (SecondIngredient >= 0)
		{
			FName Craftable = *ResourceInventory[FirstIngredient]->CheckCanCombine(ResourceInventory[SecondIngredient]);
			if (Craftable != "")
			{
				AUsable* Usable = NewObject<AUsable>();
				if (Usable)
				{
					Usable->BuildUsable(Craftable);
					UsablesInventory.Add(Usable);
					ResourceInventory.RemoveAt(FirstIngredient);
					ResourceInventory.RemoveAt(SecondIngredient - 1);

					return true;
				}
			}
		}
	}

	return false;
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

//void AMainCharacter::SortInventory()
//{
//	QuickSort(ResourceInventory, 0, ResourceInventory.Num() - 1);
//}
//
//void AMainCharacter::QuickSort(TArray<TSubclassOf<AItem>> Inventory, int Left, int Right)
//{
//	if (Right <= Left) return;
//
//	int i = 0, j = 0;
//
//	Partition(Inventory, Left, Right, i, j);
//
//	QuickSort(Inventory, Left, j);
//	QuickSort(Inventory, i, Right);
//}
//
//void AMainCharacter::Partition(TArray<TSubclassOf<AItem>> Arr, int Left, int Right, int i, int j)
//{
//	i = Left - 1;
//	j = Right;
//	int p = Left - 1, q = Right;
//	int v = Arr[Right].Value;
//
//	while (true)
//	{
//		while (Arr[++i].Value < v);
//
//		while (v < Arr[--j].Value)
//			if (j == Left)
//				break;
//
//		if (i >= j)
//			break;
//
//		Swap(i, j);
//
//		if (Arr[i].Value == v)
//		{
//			p++;
//			Swap(p, i);
//		}
//
//		if (Arr[j].Value == v)
//		{
//			q--;
//			Swap(j, q);
//		}
//	}
//
//	Swap(i, Right);
//
//	j = i - 1;
//	for (int k = Left; k < p; k++, j--)
//		Swap(k, j);
//
//	i = i + 1;
//	for (int k = Right - 1; k > q; k--, i++)
//		Swap(i, k);
//}
//
//void AMainCharacter::Swap(int Left, int Right)
//{
//	TSubclassOf<AItem> Temp = ResourceInventory[Left];
//	ResourceInventory[Left] = ResourceInventory[Right];
//	ResourceInventory[Right] = Temp;
//}

void AMainCharacter::Dodge()
{
	if (bDodging || bAttacking || bInventoryOpen || Stamina <= 0) return;

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

void AMainCharacter::ResetDodge()
{
	bAttacking = false;
	bDodging = false;
	bCanDodge = true;
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

void AMainCharacter::UseStamina(float StaminaCost, float RechargeDelay)
{
	Stamina -= StaminaCost;
	if (Stamina < 0.f) Stamina = 0.f;
	SetStaminaRechargeTimer(RechargeDelay);
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