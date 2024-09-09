// Fill out your copyright notice in the Description page of Project Settings.


#include "HealStatusEffect.h"
#include "MainCharacter.h"
#include "CharacterAttributesComponent.h"

void UHealStatusEffect::ApplyEffect()
{
	if (effectedCharacter)
	{
		if (AMainCharacter* mainCharacter = Cast<AMainCharacter>(effectedCharacter))
		{
			if (mainCharacter->mAttributes)
			{
				mainCharacter->mAttributes->AddHealth(effectValueAmount);
			}
		}
	}
}

int32 UHealStatusEffect::ModifyInt(int32 value)
{
	return int32();
}
