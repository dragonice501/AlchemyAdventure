// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UArrowComponent;
class USphereComponent;
class UBehaviorTree;
class AEnemyController;
class AMainCharacter;
class APickup;

UCLASS()
class ALCHEMYADVENTURE_API AEnemy : public ACharacter
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly) UArrowComponent* LockOnPosition;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USphereComponent* AttackSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UBehaviorTree* BehaviorTree;

	AEnemyController* EnemyController;

	float AttackMinTime = 0.5f;

	float AttackMaxTime = 1.5f;
	FTimerHandle AttackResetTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsAccelerating = false;

	bool bAttackTimerStarted = false;
	bool bInAttackRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) AMainCharacter* TargetCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LockOnBoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FTimerHandle HealthBarTimer;
	float HealthBarDisplayTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxChaseDistance;

	FVector StartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<APickup> DropItem;

	// Base Character
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") USoundBase* HitSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") bool bAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") float StunChance;
	bool bStunned = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* HurtMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* RecoilMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float Poise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float MaxPoise;
	FTimerHandle ResetPoiseRechargeTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") bool bIsDead = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AWeapon* RightHandEquipment = nullptr;
	int32 RighHandIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AWeapon* LeftHandEquipment = nullptr;
	int32 LeftHandIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AWeapon> StartingWeapon;

public:

	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable) void SetTargetCharacter(class AMainCharacter* MainCharacter, bool Seen);

	UFUNCTION(BlueprintCallable) void SetTargetLastLocation(FVector Location);

	void SetDead();

	UFUNCTION(BlueprintCallable) void ResetAttacking();

	UFUNCTION(BlueprintCallable) void StartAttack();
	void Attack();

	UFUNCTION(BlueprintCallable) void LookAtTarget();

	UFUNCTION() void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent) void ShowHealthBar(bool Locked);
	void ShowHealthBar_Implementation(bool Locked);

	UFUNCTION(BlueprintImplementableEvent) void HideHealthBar();

	// Base Character
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void DepletePoise(float Cost);
	void SetPoiseRechargeTimer();
	void ResetPoise();

	virtual void Stagger();
	void Recoil();

	UFUNCTION(BlueprintCallable) void ResetStunned();

	void Die(AActor* Causer);
	UFUNCTION(BlueprintCallable) void DeathEnd();

	UFUNCTION(BlueprintCallable) void EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip);
	UFUNCTION(BlueprintCallable) void EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip);

	UFUNCTION(BlueprintCallable) void ActivateWeapon();
	UFUNCTION(BlueprintCallable) void DeactivateWeapon();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
