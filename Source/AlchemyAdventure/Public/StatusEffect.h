// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"

class AMainCharacter;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ALCHEMYADVENTURE_API UStatusEffect : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent) void ApplyEffect(AMainCharacter* mainCharacter);
};
