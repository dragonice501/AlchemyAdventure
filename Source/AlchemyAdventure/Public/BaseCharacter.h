// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UParticleSystem;
class USoundBase;
class UAnimMontage;
class AWeapon;

UCLASS()
class ALCHEMYADVENTURE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

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
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void DepletePoise(float Cost);
	void SetPoiseRechargeTimer();
	void ResetPoise();

	virtual void Stagger();
	void Recoil();

	UFUNCTION(BlueprintCallable)
	void ResetStunned();

	virtual void Die(AActor* Causer);
	UFUNCTION(BlueprintCallable)
	virtual void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip);
	UFUNCTION(BlueprintCallable)
	void EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip);

	UFUNCTION(BlueprintCallable)
	void ActivateWeapon();

	UFUNCTION(BlueprintCallable)
	void DeactivateWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
