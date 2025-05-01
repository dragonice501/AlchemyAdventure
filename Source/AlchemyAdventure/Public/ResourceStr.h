// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "ResourceStr.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FResourceStr : public FTableRowBase
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) FString description;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FString> combinables;
};
