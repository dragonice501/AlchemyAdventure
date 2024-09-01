// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Engine/Datatable.h"
#include "Resource.generated.h"

UENUM(BlueprintType)
enum class EResourceElement : uint8
{
	ERE_None UMETA(DisplayName = "None"),
	ERE_Air UMETA(DisplayName = "Air"),
	ERE_Water UMETA(DisplayName = "Water"),
	ERE_Earth UMETA(DisplayName = "Earth"),
	ERE_Fire UMETA(DisplayName = "Fire"),
	ERE_Magic UMETA(DisplayName = "Magic"),
};

USTRUCT(BlueprintType)
struct FResourcePropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ResourceID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString ResourceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> CombinableResources;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> CombineResults;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* resourceImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EResourceElement ResourceElement;
};

UCLASS()
class ALCHEMYADVENTURE_API AResource : public AItem
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString ResourceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ResourceID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FString> CombinableResources;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FString> CombineResults;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UTexture2D* resourceImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) EResourceElement ResourceElement;

public:

	void BuildResource(FName ItemName);

	FString CheckCanCombine(AResource* OtherResource);
};
