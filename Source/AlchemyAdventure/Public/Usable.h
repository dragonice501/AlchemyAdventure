// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/Datatable.h"
#include "Usable.generated.h"

UENUM(BlueprintType)
enum class EUsableType : uint8
{
	ERE_Air UMETA(DisplayName = "Air"),
	ERE_Water UMETA(DisplayName = "Water"),
	ERE_Earth UMETA(DisplayName = "Earth"),
	ERE_Fire UMETA(DisplayName = "Fire"),
	ERE_Magic UMETA(DisplayName = "Magic"),
};

USTRUCT(BlueprintType)
struct FUsablePropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString UsableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* UsableImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUsableType UsableType;
};

UCLASS()
class ALCHEMYADVENTURE_API AUsable : public AItem
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString UsableName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTexture2D* UsableImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EUsableType UsableType;

public:

	void BuildUsable(FName UsableName);

};
