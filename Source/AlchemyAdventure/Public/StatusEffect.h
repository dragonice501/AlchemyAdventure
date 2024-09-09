// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatusEffect.generated.h"

class AMainCharacter;

UENUM(BlueprintType)
enum class EEffectDuration : uint8
{
	EED_INSTANT UMETA(DisplayName = "Instant"),
	EED_TEMPORARY UMETA(DisplayName = "Temporary"),
	EED_TIMED UMETA(DisplayName = "Timed")
};

UENUM(BlueprintType)
enum class EEffectType : uint8
{
	EED_HEALTH UMETA(DisplayName = "Health"),
	EED_ATTACK UMETA(DisplayName = "Attack"),
	EED_DEFENSE UMETA(DisplayName = "Defense"),
	EED_MOBILITY UMETA(DisplayName = "Mobility")
};

USTRUCT(BlueprintType)
struct FStatusEffectPropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* effectImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectType effectType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectDuration effectDurationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float effectDurationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float effectValueAmount;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ALCHEMYADVENTURE_API UStatusEffect : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString effectName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* effectImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectType effectType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectDuration effectDurationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float effectDurationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float effectValueAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ACharacter* effectedCharacter;
	
public:
	bool operator== (const UStatusEffect& other)
	{
		return effectName == other.effectName;
	}
	friend bool operator== (const UStatusEffect& a, const UStatusEffect& b)
	{
		return a.effectName == b.effectName;
	}
	friend uint32 GetTypeHash(const UStatusEffect& other)
	{
		return GetTypeHash(other.effectName);
	}

	void SetEffectedCharacter(ACharacter* character) { effectedCharacter = character; }
	void BuildEffect(const FStatusEffectPropertyTable* row);

	virtual void ApplyEffect();
	virtual int32 ModifyInt(int32 value);
};
