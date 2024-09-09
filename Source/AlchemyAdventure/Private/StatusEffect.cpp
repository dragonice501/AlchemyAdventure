// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffect.h"

void UStatusEffect::BuildEffect(const FStatusEffectPropertyTable* row)
{
	if (row)
	{
		effectImage = row->effectImage;
		effectType = row->effectType;
		effectDurationType = row->effectDurationType;
		effectDurationTime = row->effectDurationTime;
		effectValueAmount = row->effectValueAmount;
	}
}

void UStatusEffect::ApplyEffect()
{
}

int32 UStatusEffect::ModifyInt(int32 value)
{
	return value + effectValueAmount;
}
