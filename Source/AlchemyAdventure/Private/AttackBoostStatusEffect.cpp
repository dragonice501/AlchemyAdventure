// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackBoostStatusEffect.h"

void UAttackBoostStatusEffect::ApplyEffect()
{
}

int32 UAttackBoostStatusEffect::ModifyInt(int32 value)
{
	return value + effectValueAmount;
}
