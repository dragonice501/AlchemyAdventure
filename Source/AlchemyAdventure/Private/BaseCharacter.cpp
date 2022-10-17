// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MainCharacter.h"
#include "Enemy.h"
#include "Weapon.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (StartingWeapon)
	{
		EquipRightHand(StartingWeapon);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
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

		float StunValue = FMath::FRandRange(0.f, 100.f);
		if (StunValue < 75.f)
		{
			Stun();
		}
	}

	return DamageAmount;
}

void ABaseCharacter::Stun()
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

void ABaseCharacter::ResetStunned()
{
	bStunned = false;
}

void ABaseCharacter::Recoil()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RecoilMontage)
	{
		AnimInstance->Montage_Play(RecoilMontage);
		AnimInstance->Montage_JumpToSection("");
	}
}

void ABaseCharacter::EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip)
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

void ABaseCharacter::EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip)
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

void ABaseCharacter::ActivateWeapon()
{
	if (RightHandEquipment)
	{
		RightHandEquipment->ActivateAttackCollision();
	}
}

void ABaseCharacter::DeactivateWeapon()
{
	if (RightHandEquipment)
	{
		RightHandEquipment->DeactivateAttackCollision();
	}
}

void ABaseCharacter::Die(AActor* Causer)
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

void ABaseCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	Destroy();
}