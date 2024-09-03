// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MainCharacter.h"
#include "Weapon.h"
#include "Pickup.h"
#include "Engine/SkeletalMeshSocket.h"

AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LockOnPosition = CreateDefaultSubobject<UArrowComponent>(TEXT("LockOnPosition"));
	LockOnPosition->SetupAttachment(RootComponent);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
	AttackSphere->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AEnemyController>(GetController());

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackSphereBeginOverlap);
	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AttackSphereEndOverlap);

	StartPosition = GetActorLocation();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetCharacter)
	{
		float Distance = FVector::DistSquared(GetActorLocation(), StartPosition);
		if (Distance > MaxChaseDistance * MaxChaseDistance)
		{
			SetTargetCharacter(TargetCharacter, false);
		}
	}
}

void AEnemy::SetTargetCharacter(class AMainCharacter* MainCharacter, bool Seen)
{
	if (MainCharacter)
	{
		TargetCharacter = MainCharacter;
		if (EnemyController)
		{
			if (Seen)
			{
				EnemyController->BlackboardComponent->SetValueAsObject(TEXT("TargetActor"), MainCharacter);
			}
			else
			{
				EnemyController->BlackboardComponent->ClearValue(TEXT("TargetActor"));
			}
		}
	}
}

void AEnemy::SetTargetLastLocation(FVector Location)
{
	if (EnemyController)
	{
		EnemyController->BlackboardComponent->SetValueAsVector(TEXT("TargetLastLocation"), Location);
	}
}

void AEnemy::SetDead()
{
	if (EnemyController)
	{
		EnemyController->EndBehaviorTree();
	}
}

void AEnemy::DeathEnd()
{
	if (RightHandEquipment)
	{
		RightHandEquipment->Destroy();
	}

	if (DropItem)
	{
		FHitResult HitResult;
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation() + FVector::DownVector * 1000;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
		if (HitResult.bBlockingHit)
		{
			FVector DropPosition = HitResult.Location;
			AActor* Drop = GetWorld()->SpawnActor(DropItem);
			Drop->SetActorLocationAndRotation(DropPosition, FQuat{ 0 });
		}
		
	}

	GetMesh()->bPauseAnims = true;
	Destroy();
}

void AEnemy::AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			bInAttackRange = true;
			if (EnemyController)
			{
				EnemyController->BlackboardComponent->SetValueAsBool(TEXT("InAttackRange"), true);
			}
		}
	}
}

void AEnemy::AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			bInAttackRange = false;
			if (EnemyController)
			{
				EnemyController->BlackboardComponent->SetValueAsBool(TEXT("InAttackRange"), false);
			}
		}
	}
}

void AEnemy::ResetAttacking()
{
	bAttacking = false;
}

void AEnemy::StartAttack()
{
	if (bAttacking || bStunned) return;

	bAttacking = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.f);
		AnimInstance->Montage_JumpToSection(TEXT("Attack1"), AttackMontage);
	}
}

void AEnemy::LookAtTarget()
{
	FVector TargetDirection = TargetCharacter->GetActorLocation() - GetActorLocation();
	TargetDirection.Z = 0.f;
	FRotator LookRotation = TargetDirection.Rotation();
	SetActorRotation(LookRotation);
}

void AEnemy::ShowHealthBar_Implementation(bool Locked)
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemy::HideHealthBar, HealthBarDisplayTime);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(this);
	if (MainCharacter)
	{
		if (MainCharacter->bInvincible == true) return 0;
	}

	AEnemy* Enemy = Cast<AEnemy>(this);
	if (Enemy)
	{
		Enemy->SetTargetCharacter(Cast<AMainCharacter>(EventInstigator->GetCharacter()), true);
	}

	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die(DamageCauser);
		return DamageAmount;
	}
	else
	{
		Health -= DamageAmount;
	}

	AWeapon* Weapon = Cast<AWeapon>(DamageCauser);
	if (Weapon)
	{
		DepletePoise(Weapon->poiseCost);
	}

	return DamageAmount;
}

void AEnemy::DepletePoise(float Cost)
{
	Poise -= Cost;
	if (Poise <= 0.f)
	{
		Poise = MaxPoise;
		Stagger();
		return;
	}
	SetPoiseRechargeTimer();
}

void AEnemy::SetPoiseRechargeTimer()
{
	GetWorldTimerManager().ClearTimer(ResetPoiseRechargeTimer);
	GetWorldTimerManager().SetTimer(ResetPoiseRechargeTimer, this, &AEnemy::ResetPoise, 5.0f);
}

void AEnemy::ResetPoise()
{
	Poise = MaxPoise;
}

void AEnemy::Stagger()
{
	bStunned = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HurtMontage)
	{
		AnimInstance->Montage_Play(HurtMontage);
		AnimInstance->Montage_JumpToSection(TEXT("HurtFront"));
	}

	bAttacking = false;
}

void AEnemy::ResetStunned()
{
	bStunned = false;
}

void AEnemy::Recoil()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RecoilMontage)
	{
		AnimInstance->Montage_Play(RecoilMontage);
		AnimInstance->Montage_JumpToSection("");
	}
}

void AEnemy::EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip)
{
	if (WeaponToEquip)
	{
		if (RightHandEquipment != nullptr)
		{
			RightHandEquipment->Destroy();
		}

		AActor* Weapon = GetWorld()->SpawnActor(WeaponToEquip);
		const USkeletalMeshSocket* RightHandSocket = GetMesh()->GetSocketByName("Weapon_R");
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(Weapon, GetMesh());
		}

		RightHandEquipment = Cast<AWeapon>(Weapon);
		if (RightHandEquipment)
		{
			RightHandEquipment->SetMainCharacter(this);
		}
	}
	else if (WeaponToEquip == nullptr)
	{
		if (RightHandEquipment != nullptr)
		{
			RightHandEquipment->Destroy();
		}
	}
}

void AEnemy::EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip)
{
	if (WeaponToEquip)
	{
		if (LeftHandEquipment != nullptr)
		{
			LeftHandEquipment->Destroy();
		}

		AActor* Weapon = GetWorld()->SpawnActor(WeaponToEquip);
		const USkeletalMeshSocket* LeftHandSocket = GetMesh()->GetSocketByName("Weapon_L");
		if (LeftHandSocket)
		{
			LeftHandSocket->AttachActor(Weapon, GetMesh());
		}

		LeftHandEquipment = Cast<AWeapon>(Weapon);
		if (LeftHandEquipment)
		{
			LeftHandEquipment->SetMainCharacter(this);
		}
	}
	else if (WeaponToEquip == nullptr)
	{
		if (LeftHandEquipment != nullptr)
		{
			LeftHandEquipment->Destroy();
		}
	}
}

void AEnemy::ActivateWeapon()
{
	if (RightHandEquipment)
	{
		RightHandEquipment->ActivateAttackCollision();
	}
}

void AEnemy::DeactivateWeapon()
{
	if (RightHandEquipment)
	{
		RightHandEquipment->DeactivateAttackCollision();
	}
}

void AEnemy::Die(AActor* Causer)
{
	AEnemy* Enemy = Cast<AEnemy>(this);
	if (Enemy)
	{
		Enemy->SetDead();
	}

	AWeapon* Weapon = Cast<AWeapon>(Causer);
	if (Weapon)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(Weapon->GetCharacter());
		if (MainCharacter)
		{
			MainCharacter->UntargetEnemy();
			MainCharacter->SwitchMovementStyle(EMovementStyle::EMS_Free);
			MainCharacter->SwitchCameraMovement(ECameraMovement::ECM_Free);
		}
	}

	UAnimInstance* AnimInstace = GetMesh()->GetAnimInstance();
	if (AnimInstace && DeathMontage)
	{
		AnimInstace->Montage_Play(DeathMontage, 1.0);
		AnimInstace->Montage_JumpToSection(FName("Death"), DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bAttacking = false;
}