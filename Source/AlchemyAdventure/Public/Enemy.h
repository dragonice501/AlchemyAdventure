// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class ALCHEMYADVENTURE_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UArrowComponent* LockOnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USphereComponent* AttackSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBehaviorTree* BehaviorTree;

	class AEnemyController* EnemyController;

	float AttackMinTime = 0.5f;

	float AttackMaxTime = 1.5f;
	FTimerHandle AttackResetTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAccelerating = false;

	bool bAttackTimerStarted = false;
	bool bInAttackRange = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMainCharacter* TargetCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LockOnBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTimerHandle HealthBarTimer;
	float HealthBarDisplayTime = 2.0f;

public:

	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetTargetCharacter(class AMainCharacter* MainCharacter, bool Seen);

	UFUNCTION(BlueprintCallable)
	void SetTargetLastLocation(FVector Location);

	/*virtual void Die(AActor* Causer) override;*/
	virtual void DeathEnd() override;
	void SetDead();

	UFUNCTION(BlueprintCallable)
	void ResetAttacking();

	UFUNCTION(BlueprintCallable)
	void StartAttack();
	void Attack();

	UFUNCTION(BlueprintCallable)
	void LookAtTarget();

	UFUNCTION()
	void AttackSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AttackSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar(bool Locked);
	void ShowHealthBar_Implementation(bool Locked);

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
