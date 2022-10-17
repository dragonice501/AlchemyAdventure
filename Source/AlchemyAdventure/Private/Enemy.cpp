// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MainCharacter.h"
#include "Weapon.h"
#include "Pickup.h"

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

void AEnemy::Die(AActor* Causer)
{
	Super::Die(Causer);
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

	Super::DeathEnd();
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