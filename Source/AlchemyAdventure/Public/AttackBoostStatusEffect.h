// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "AttackBoostStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class ALCHEMYADVENTURE_API UAttackBoostStatusEffect : public UStatusEffect
{
	GENERATED_BODY()
public:

	void ApplyEffect() override;
	int32 ModifyInt(int32 value) override;
};
