// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"

// Sets default values for this component's properties
UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	// ...
}


// Called when the game starts
void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	staminaTimer.BindDynamic(this, &UCharacterAttributesComponent::ResetStaminaRecharge);
}


// Called every frame
void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (staminaCanRecharge && stamina < maxStamina)
	{
		stamina += staminaRechargeRate * DeltaTime;
		if (stamina > maxStamina) stamina = maxStamina;
	}
}

void UCharacterAttributesComponent::SetStaminaRechargeTimer(float time)
{
	staminaCanRecharge = false;
	GetWorld()->GetTimerManager().SetTimer(resetStaminaRechargeTimer, this, &UCharacterAttributesComponent::ResetStaminaRecharge, time);
}

void UCharacterAttributesComponent::ResetStaminaRecharge()
{
	staminaCanRecharge = true;
}

void UCharacterAttributesComponent::UseStamina(float staminaCost)
{
	stamina -= staminaCost;
	if (stamina < 0.0f) stamina = 0.0f;
}

void UCharacterAttributesComponent::SetPoiseRechargeTimer(float time)
{
	staminaCanRecharge = false;
	GetWorld()->GetTimerManager().SetTimer(resetPoiseRechargeTimer, this, &UCharacterAttributesComponent::ResetPoiseRecharge, time);
}

void UCharacterAttributesComponent::ResetPoiseRecharge()
{
	poise = maxPoise;
}

void UCharacterAttributesComponent::DepletePoise(float cost)
{
	poise -= cost;
	if (poise <= 0.0f) poise = maxPoise;
}
