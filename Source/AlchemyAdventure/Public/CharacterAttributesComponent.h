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
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StaminaRechargeRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Poise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxPoise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bIsDead = false;
	bool bStaminaCanRecharge = true;
	FTimerHandle ResetStaminaRechargeTimer;
	FTimerHandle ResetPoiseRechargeTimer;

public:	
	// Sets default values for this component's properties
	UCharacterAttributesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
