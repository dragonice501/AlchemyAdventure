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

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* usableImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EUsableElement usableElement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EStatusEffect statusEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 statusEffectTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float attackModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float healAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float defenseModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float mobilityModifier;
};

UCLASS()
class ALCHEMYADVENTURE_API AUsable : public AItem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") FString startingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") FString usableName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") UTexture2D* usableImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") EUsableElement usableType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") EStatusEffect statusEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") int32 statusEffectTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") float attackModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") float healAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") float defenseModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Usable") float mobilityModifier;

public:

	void BuildUsable(FName UsableName);
	void BuildUsable(const FUsablePropertyTable* row);
};
