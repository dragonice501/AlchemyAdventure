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

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::BuildWeapon()
{

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBeginOverlap);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
					int32 FinalDamage = Damage;
					if (OwningCharacter->bAttackModifier)
					{
						FinalDamage *= OwningCharacter->AttackModifier;
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
					int32 FinalDamage = Damage;
					if (MainCharacter->bDefenseModifier)
					{
						FinalDamage *= MainCharacter->DefenseModifier;
					}
					UGameplayStatics::ApplyDamage(MainCharacter, FinalDamage, Owner->GetController(), this, DamageType);
					MainCharacter->ResetDodge();
				}
				HitCharacters.Add(MainCharacter);
			}
		}
	}
}

void AWeapon::ActivateCollision()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::DeactivateCollision()
{
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCharacters.Empty();
}