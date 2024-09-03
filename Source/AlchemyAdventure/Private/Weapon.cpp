// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "MainCharacter.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SetRootComponent(ArrowComponent);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);
	AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	AttackCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	GuardCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GuardCollisionBox"));
	GuardCollisionBox->SetupAttachment(RootComponent);
	GuardCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GuardCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GuardCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GuardCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
}

void AWeapon::BuildWeapon()
{
	
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnAttackBeginOverlap);
	GuardCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnGuardBeginOverlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* OwningEnemy = Cast<AEnemy>(Owner);
		AMainCharacter* OwningCharacter = Cast<AMainCharacter>(Owner);

		if (OwningCharacter)
		{
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);

			if (HitCharacters.Contains(Enemy)) return;

			if (Enemy)
			{
				if (DamageType && Owner)
				{
					int32 FinalDamage = damage;
					if (OwningCharacter->bAttackModifier)
					{
						FinalDamage *= OwningCharacter->attackModifier;
					}

					UGameplayStatics::ApplyDamage(Enemy, FinalDamage, Owner->GetController(), this, DamageType);
					if (Enemy->Health == 0)
					{
						OwningCharacter->FindBestTargetEnemy();
					}

					if (OwningCharacter->TargetEnemy != Enemy)
					{
						Enemy->ShowHealthBar(false);
					}
				}
				HitCharacters.Add(Enemy);
			}
		}
		else if (OwningEnemy)
		{
			AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
			if (MainCharacter)
			{
				if (HitCharacters.Contains(MainCharacter)) return;

				if (MainCharacter->bInvincible)
				{
					return;
				}

				if (DamageType && Owner)
				{
					int32 FinalDamage = damage;
					if (MainCharacter->bDefenseModifier)
					{
						FinalDamage *= MainCharacter->defenseModifier;
					}
					UGameplayStatics::ApplyDamage(MainCharacter, FinalDamage, Owner->GetController(), this, DamageType);
					MainCharacter->ResetDodge();
				}
				HitCharacters.Add(MainCharacter);
			}
		}
	}
}

void AWeapon::OnGuardBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AWeapon* OtherWeapon = Cast<AWeapon>(OtherActor);
		if (OtherWeapon)
		{
			OtherWeapon->DeactivateAttackCollision();
			AMainCharacter* MainCharacter = Cast<AMainCharacter>(Owner);
			if (MainCharacter)
			{
				MainCharacter->BlockHit();
			}
		}
	}
}

void AWeapon::ActivateAttackCollision()
{
	AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateAttackCollision()
{
	AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCharacters.Empty();
}

void AWeapon::ActivateGuardCollision()
{
	GuardCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateGuardCollision()
{
	GuardCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlockedWeapons.Empty();
}