// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class AResource;
class AUsable;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALCHEMYADVENTURE_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AResource*> resourceInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> usablesInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TSubclassOf<AWeapon>> weaponInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TSubclassOf<AUsable>> PotionMap;

	// Gear
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32> currentGearIndexes{ -1, -1, -1, -1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> gearSlotOneInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> gearSlotTwoInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> gearSlotThreeInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> gearSlotFourInventory;

	// Crafting
	UPROPERTY(VisibleAnywhere) TArray<AResource*> setIngredientsOneInv;
	UPROPERTY(VisibleAnywhere) TArray<AResource*> setIngredientsTwoInv;

public:	
	// Sets default values for this component's properties
	UPlayerInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Inventory
	void AddToResourceInventory(AResource* resource) { resourceInventory.Add(resource); }
	void AddToUsuablesInventory(AUsable* usable) { usablesInventory.Add(usable); }
	void AddToWeaponInventory(TSubclassOf<AWeapon> weapon);

	//void SortResourceInventory() { resourceInventory.Sort(); }

	UFUNCTION(BlueprintCallable) bool RemoveAndSetIngredient(int32 resourceStackIndex, int32 resourceSelectIndex, UTexture2D*& resourceImage);
	UFUNCTION(BlueprintCallable) void ResetCraftingIngredients(bool resetFirst, bool resetSecond);

	UFUNCTION(BlueprintCallable) void GetResource(int32 resourceStackIndex, int32 inUseIngredientIndex, int32& resourceInventoryIndex, UTexture2D*& resourceImage, bool& hasResource);
	UFUNCTION(BlueprintCallable) void GetResourceImage(int32 resourceStackIndex, UTexture2D*& resourceImage);
	UFUNCTION(BlueprintCallable) void GetResourceCount(int32 resourceStackIndex, int32& resourceCount);

	UFUNCTION(BlueprintCallable) UTexture2D* CheckCanCraft();
	UFUNCTION(BlueprintCallable) bool AddUsable();
	UFUNCTION(BlueprintCallable) bool CheckCanCraftMore();
	UFUNCTION(BlueprintCallable) int32 GetIngredientsCount(bool first, bool second);
	UFUNCTION(BlueprintCallable) UTexture2D* GetIngredientImage(bool first, bool second);

	UFUNCTION(BlueprintCallable) UTexture2D* GetEquipmentImage(int32 index);
	UFUNCTION(BlueprintCallable) UTexture2D* GetGearImage(int32 index, int32& itemCount);

	// Gear
	UFUNCTION(BlueprintCallable) void SetGearUseIndex(int32 gearBoxIndex, int32 desiredInventoryIndex);
	UFUNCTION(BlueprintCallable) void GetGearStack(int32 stackIndex, int32 slotIndex);
	UFUNCTION(BlueprintCallable) void RemoveGearStack(int32 slotIndex);
	UFUNCTION(BlueprintCallable) void SwapGearSlot(int32 firstSlot, int32 secondSlot);
	UFUNCTION(BlueprintCallable) void GetGearSlotImageAndCount(int32 slotIndex, UTexture2D*& outImage, int32& count, bool& hasGear);
	UFUNCTION(BlueprintCallable) void GetGearInventoryStackImageAndCount(int32 stackIndex, UTexture2D*& outImage, int32& count, bool& hasGear);
	void AddToGearSlot(AUsable* usableToAdd, int32 slotIndex);

	/*void QuickSortUsables(TArray<AUsable*> Inventory, int32 Low, int32 High);
	int32 Partition(TArray<AUsable*> Inventory, int32 Low, int32 High);
	void Swap(int32 i, int32 j);
	void DutchQuickSort(TArray<AUsable*> Inventory, int Left, int Right);
	void DutchPartition(TArray<AUsable*> Arr, int Left, int Right, int i, int j);*/
};
