// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusEffect.h"
#include "StatusEffectsComponent.generated.h"

//UDELEGATE(BlueprintCallable)
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStatusEffectAppliedDelegate, StatusEffect*, statusEffect);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALCHEMYADVENTURE_API UStatusEffectsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Editanywhere, BlueprintReadWrite) TArray<UStatusEffect*> mStatusEffects;

public:	
	// Sets default values for this component's properties
	UStatusEffectsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddStatusEffect(FDataTableRowHandle statusEffect, ACharacter* effectedCharacter);
	void RemoveStatusEffect(int32 statusEffectIndex);

	void ModifyAttackDamage(int32& damage);
};
