// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pickup.h"
#include "Resource.h"
#include "Usable.h"
#include "Weapon.h"
#include "PlayerInventoryComponent.generated.h"

class UStatusEffect;

USTRUCT(BlueprintType)
struct FWeaponPropertyTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AWeapon> weaponBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* weaponImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeaponType weaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaRechargeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float poiseCost;
};

USTRUCT(BlueprintType)
struct FResourceStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString resourceName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString resourceDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> combinableResources;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FString> combineResults;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* resourceImage;

	FResourceStruct() : resourceName(""), resourceImage(nullptr) {}

	bool operator== (const FResourceStruct& other)
	{
		return resourceName == other.resourceName;
	}
	friend bool operator== (const FResourceStruct& a, const FResourceStruct& b)
	{
		return a.resourceName == b.resourceName;
	}
	friend uint32 GetTypeHash(const FResourceStruct& other)
	{
		return GetTypeHash(other.resourceName);
	}

	void BuildResource(const FDataTableRowHandle& datatable)
	{
		if (FResourceStruct* row = datatable.GetRow<FResourceStruct>(datatable.RowName.ToString()))
		{
			resourceName = datatable.RowName.ToString();
			resourceDescription = row->resourceDescription;
			combinableResources = row->combinableResources;
			combineResults = row->combineResults;
			resourceImage = row->resourceImage;
		}
	}
};

USTRUCT(BlueprintType)
struct FU
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString usableDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString montageSection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* usableImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDataTableRowHandle statusEffectDataTable;

	FU() : usableName(""), usableImage(nullptr) {}

	bool operator== (const FU& other)
	{
		return usableName == other.usableName;
	}
	friend bool operator== (const FU& a, const FU& b)
	{
		return a.usableName == b.usableName;
	}
	friend uint32 GetTypeHash(const FU& other)
	{
		return GetTypeHash(other.usableName);
	}

	void BuildUsable(const FUsablePropertyTable* row)
	{
		if (row)
		{
			usableName = row->usableName;
			usableDescription = row->usableDescription;
			montageSection = row->montageSection;
			usableImage = row->usableImage;
			statusEffectDataTable = row->statusEffect;
		}
	}
};

USTRUCT(BlueprintType)
struct FW
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AWeapon> weaponBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString weaponName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D* weaponImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeaponType weaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float staminaRechargeDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float poiseCost;

	FW() : weaponName(""), weaponType(EWeaponType::EWT_Sword) {}

	bool operator== (const FW& other)
	{
		return weaponName == other.weaponName;
	}
	friend bool operator== (const FW& a, const FW& b)
	{
		return a.weaponName == b.weaponName;
	}
	friend uint32 GetTypeHash(const FW& other)
	{
		return GetTypeHash(other.weaponName);
	}

	void BuildWeapon(const FWeaponPropertyTable* row, const FName& name)
	{
		if (row)
		{
			weaponBlueprint = row->weaponBlueprint;
			weaponName = name.ToString();
			weaponImage = row->weaponImage;
			weaponType = row->weaponType;
			damage = row->damage;
			staminaCost = row->staminaCost;
			staminaRechargeDelay = row->staminaRechargeDelay;
			poiseCost = row->poiseCost;
		}
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALCHEMYADVENTURE_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FResourceStruct, uint8> resourceMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FResourceStruct, uint8> firstIngredientMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FResourceStruct, uint8> secondIngredientMap;


	// Crafting
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FDataTableRowHandle usableDataTable;

	// Gear
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FW> weaponMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FU, uint8> usableMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FU, uint8> gearSlotOneMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FU, uint8> gearSlotTwoMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FU, uint8> gearSlotThreeMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TMap<FU, uint8> gearSlotFourMap;

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
	void AddToResourceInventory(const FDataTableRowHandle& itemDataTable);
	void AddToResourceInventory(const FResourceStruct& resource, uint8 count);
	void AddToUsablesInventory(const FU& usable, const uint8 count);
	void AddToWeaponInventory(const FDataTableRowHandle& weaponDataTable);

	void RemoveResourceFromInventory(const FResourceStruct& resource, uint8 count);

	UFUNCTION(BlueprintCallable) bool RemoveAndSetIngredient(int32 resourceStackIndex, int32 resourceSelectIndex, bool firstIngredient, UTexture2D*& resourceImage);
	UFUNCTION(BlueprintCallable) void ResetCraftingIngredients(bool resetFirst, bool resetSecond);

	UFUNCTION(BlueprintCallable) void GetResourceImage(int32 resourceStackIndex, UTexture2D*& resourceImage);
	UFUNCTION(BlueprintCallable) void GetResourceCount(int32 resourceStackIndex, int32& resourceCount);

	UFUNCTION(BlueprintCallable) UTexture2D* CheckCanCraft();
	UFUNCTION(BlueprintCallable) bool AddUsable();
	UFUNCTION(BlueprintCallable) bool CheckCanCraftMore();
	UFUNCTION(BlueprintCallable) UTexture2D* GetIngredientImage(bool first, bool second);
	UFUNCTION(BlueprintCallable) int32 GetIngredientsCount(bool first, bool second);
	UFUNCTION(BlueprintCallable) UTexture2D* GetInventoryWeaponImage(int32 index);

	// Gear
	UFUNCTION(BlueprintCallable) void SetGearUseIndex(int32 gearBoxIndex, int32 desiredInventoryIndex);
	UFUNCTION(BlueprintCallable) void SetGearSlot(int32 stackIndex, int32 slotIndex);
	UFUNCTION(BlueprintCallable) void EmptyGearAtSlot(int32 gearSlotIndex);
	UFUNCTION(BlueprintCallable) void RemoveFromGearSlot(int32 gearSlotIndex, int32 count);
	UFUNCTION(BlueprintCallable) void UseGearInSlot(int32 gearSlotIndex);
	UFUNCTION(BlueprintCallable) void GetGearSlotImageAndCount(int32 slotIndex, UTexture2D*& outImage, int32& count, bool& hasGear);
	UFUNCTION(BlueprintCallable) void GetGearInventoryStackImageAndCount(int32 stackIndex, UTexture2D*& outImage, int32& count, bool& hasGear);
	FDataTableRowHandle GetGearStatusEffect(int32 gearSlotIndex);
};
