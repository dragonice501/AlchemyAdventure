// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
//#include "StatusEffect.h"
#include "Usable.generated.h"

class UStatusEffect;

UENUM(BlueprintType)
enum class EEffectDuration : uint8
{
	EED_INSTANT UMETA(DisplayName = "Instant"),
	EED_TEMPORARY UMETA(DisplayName = "Temporary"),
	EED_TIMED UMETA(DisplayName = "Timed")
};

//USTRUCT(BlueprintType)
//struct FStatusEffect
//{
//	GENERATED_BODY()
//};
//
//USTRUCT(BlueprintType)
//struct FHealEffect : public FStatusEffect
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite) EEffectDuration effectDuration;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite) float healAmount;
//
//	FHealEffect()
//	{
//		
//	}
//
//	void ApplyStatusEffect()
//	{
//
//	}
//};

USTRUCT(BlueprintType)
struct FUsablePropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString montageSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* usableImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UStatusEffect> statusEffect;
};

UCLASS()
class ALCHEMYADVENTURE_API AUsable : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") FString startingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") FString usableName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") UTexture2D* usableImage;

public:

	void BuildUsable(FName UsableName);
	void BuildUsable(const FUsablePropertyTable* row);
};
