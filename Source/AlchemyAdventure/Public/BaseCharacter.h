// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ALCHEMYADVENTURE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundBase* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	float StunChance;
	bool bStunned = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* HurtMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 XP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AWeapon* RightHandEquipment = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AWeapon* LeftHandEquipment = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AWeapon> StartingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Stun();

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
