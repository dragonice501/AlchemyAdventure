// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatusEffect.h"
#include "HealStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class ALCHEMYADVENTURE_API UHealStatusEffect : public UStatusEffect
{
	GENERATED_BODY()
	
	void ApplyEffect() override;
	int32 ModifyInt(int32 value) override;
};
