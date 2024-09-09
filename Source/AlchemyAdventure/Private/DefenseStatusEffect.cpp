// Fill out your copyright notice in the Description page of Project Settings.


#include "DefenseStatusEffect.h"

void UDefenseStatusEffect::ApplyEffect()
{
}

int32 UDefenseStatusEffect::ModifyInt(int32 value)
{
	return value + effectValueAmount;
}
