// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributesComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALCHEMYADVENTURE_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float maxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float maxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaRechargeRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float poise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float maxPoise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool staminaCanRecharge = true;
	FTimerDynamicDelegate staminaTimer;
	FTimerDynamicDelegate poiseTimer;
	FTimerHandle resetStaminaRechargeTimer;
	FTimerHandle resetPoiseRechargeTimer;

public:	
	// Sets default values for this component's properties
	UCharacterAttributesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable) void AddHealth(const float amount) { health = FMath::Clamp(health + amount, 0, maxHealth); }

	void SetStaminaRechargeTimer(float time);
	void ResetStaminaRecharge();
	void UseStamina(float staminaCost);

	void SetPoiseRechargeTimer(float time);
	void ResetPoiseRecharge();
	void DepletePoise(float cost);
};
