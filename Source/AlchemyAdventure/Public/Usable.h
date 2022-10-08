// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/Datatable.h"
#include "Usable.generated.h"

UENUM(BlueprintType)
enum class EUsableElement : uint8
{
	ERE_None UMETA(DisplayName = "None"),
	ERE_Air UMETA(DisplayName = "Air"),
	ERE_Water UMETA(DisplayName = "Water"),
	ERE_Earth UMETA(DisplayName = "Earth"),
	ERE_Fire UMETA(DisplayName = "Fire"),
	ERE_Magic UMETA(DisplayName = "Magic"),
};

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	ESE_None UMETA(DisplayName = "None"),
	ESE_Attack UMETA(DisplayName = "Attack"),
	ESE_Heal UMETA(DisplayName = "Heal"),
	ESE_Defense UMETA(DisplayName = "Defense"),
	ESE_Mobility UMETA(DisplayName = "Mobility"),
};

USTRUCT(BlueprintType)
struct FUsablePropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsableID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UsableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* UsableImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUsableElement UsableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStatusEffect StatusEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StatusEffectTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DefenseModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MobilityModifier;
};

UCLASS()
class ALCHEMYADVENTURE_API AUsable : public AItem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 UsableID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString StartingName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString UsableName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTexture2D* UsableImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUsableElement UsableType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EStatusEffect StatusEffect;

	int32 StatusEffectTime;

	float AttackModifier;
	float HealAmount;
	float DefenseModifier;
	float MobilityModifier;

public:

	void BuildUsable(FName UsableName);
};
