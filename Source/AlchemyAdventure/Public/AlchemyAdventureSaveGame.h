// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AlchemyAdventureSaveGame.generated.h"

class AWeapon;
class AResource;
class AUsable;

UCLASS()
class ALCHEMYADVENTURE_API UAlchemyAdventureSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UAlchemyAdventureSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic) FString PlayerName;
	UPROPERTY(VisibleAnywhere, Category = Basic) uint32 UserSlot;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic) FString LevelToLoad;
	UPROPERTY(VisibleAnywhere, Category = Basic) FVector PlayerLocation;
	UPROPERTY(VisibleAnywhere, Category = Basic) FRotator PlayerRotation;
	UPROPERTY(VisibleAnywhere, Category = Basic) float Health;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<TSubclassOf<AWeapon>> EquipmentInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) int32 RightHandIndex = -1;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AResource*> ResourceInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AUsable*> UsablesInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AUsable*> GearSlotOneInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AUsable*> GearSlotTwoInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AUsable*> GearSlotThreeInventory;
	UPROPERTY(VisibleAnywhere, Category = Basic) TArray<AUsable*> GearSlotFourInventory;
};
