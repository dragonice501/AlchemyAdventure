// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.generated.h"

class UArrowComponent;
class UBoxComponent;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Sword UMETA(DisplayName = "Sword"),
	EWT_Spear UMETA(DisplayName = "Spear"),
};

UCLASS()
class ALCHEMYADVENTURE_API AWeapon : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UArrowComponent* ArrowComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UBoxComponent* AttackCollisionBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UBoxComponent* GuardCollisionBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UDamageType> damageType;

	ACharacter* Owner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeaponType weaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* weaponImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaRechargeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float poiseCost;
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	void BuildWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnGuardBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActivateAttackCollision();
	void DeactivateAttackCollision();
	void ActivateGuardCollision();
	void DeactivateGuardCollision();

	TArray<ACharacter*> HitCharacters;
	TArray<AWeapon*> BlockedWeapons;

	FORCEINLINE ACharacter* GetCharacter() const { return Owner; }

	FORCEINLINE void SetMainCharacter(ACharacter* Character) { Owner = Character; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
