// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString resourceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString resourceDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> combinableResources;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> combineResults;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* resourceImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EResourceElement resourceElement;
};

UCLASS(BlueprintType)
class ALCHEMYADVENTURE_API AResource : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString resourceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString resourceDescription;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FString> combinableResources;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FString> combineResults;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UTexture2D* resourceImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) EResourceElement resourceElement;

public:

	void BuildResource(const FResourcePropertyTable* row);
	void BuildResource(FName ItemName);

	FString CheckCanCombine(AResource* OtherResource);
};
