// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSetImageAndCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastOpenCharacterMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastResetInventoryHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetAttackTimer, bool, bToggleAttackTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetDefenseTimer, bool, bToggleDefenseTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetMobilityTimer, bool, bToggleMobilityTimer);

class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class AMainPlayerController;
class AEnemy;
class AItem;
class APickup;
class AResource;
class AUsable;

enum class EMovementStyle : uint8
{
	EMS_Free,
	EMS_LockedOn
};

enum class ECameraMovement : uint8
{
	ECM_Free,
	EMC_LockedOn,
	EMC_Inventory
};

UCLASS()
class ALCHEMYADVENTURE_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpringArmDefaultLength = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* EnemyDetectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	AMainPlayerController* MainPlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* UseMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	UAnimMontage* GuardMontage;

	FVector InputVector;

	AEnemy* TargetEnemy = nullptr;
	FVector EnemyLockOnPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotatingActorRotate = 180.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bLockedOn = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bUsing = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanCombo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ComboCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItem>PickupItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AWeapon>> EquipmentInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AResource*> ResourceInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AUsable*> UsablesInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AUsable>> StartingUsables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AUsable>> PotionMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<APickup*> OverlappingPickups;
	APickup* CurrentPickup;
	bool bCanPickup = false;
	int32 PickupsCount = 0;

	int32 EquippedWeaponIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>CurrentGearIndexes{ -1, -1, -1, -1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> GearSlotOneInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> GearSlotTwoInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> GearSlotThreeInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<AUsable*> GearSlotFourInventory;

	float MaxWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxLockOnDistance = 2000.f;
	bool bFaceTarget = true;
	bool bCanDodge = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bDodging = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) bool bInvincible = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool bBlocking = false;

	bool bInventoryOpen = false;
	FRotator CameraFixedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Stamina = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float StaminaRechargeRate = 25.f;

	bool bStaminaCanRecharge = true;

	FTimerHandle ResetStaminaRechargeTimer;

	//Status Effects
	AUsable* DesiredGearToUse = nullptr;
	int32 DesiredGearSlot = -1;
	int32 StatusEffectTime;

	FTimerHandle AttackModifierTimer;
	bool bAttackModifier = false;
	float AttackModifier = 1.f;

	FTimerHandle DefenseModifierTimer;
	bool bDefenseModifier = false;
	float DefenseModifier = 1.f;

	FTimerHandle MobilityModifierTimer;
	bool bMobilityModifier = false;
	float MobilityModifier = 1.f;

	AResource* SetIngredientOne;
	AResource* SetIngredientTwo;
	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsOneInv;
	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsTwoInv;
	int32 SetIngredientOneIndex;
	int32 SetIngredientTwoIndex;
	FName UsedIngredientOneName;
	FName UsedIngredientTwoName;

	//Delegates
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastSetImageAndCount DynamicMulticastSetImageAndCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastResetInventoryHUD DynamicMulticastResetInventoryHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastOpenCharacterMenu DynamicMulticastOpenCharacterMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastSetAttackTimer DynamicMulticastSetAttackTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastSetDefenseTimer DynamicMulticastSetDefenseTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastSetMobilityTimer DynamicMulticastSetMobilityTimer;

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE float GetHealth() { return Health; }
	FORCEINLINE float GetMaxHealth() { return MaxHealth; }

	void SetHealth(float Amount);
	FORCEINLINE void SetMaxHealth(float Amount) { MaxHealth = Amount; }

	void CheckOverlappingPickups();
	void SetPickups(APickup* Pickup, bool Overlapped);

	bool FindBestTargetEnemy();
	void LockOnEnemy();
	void UntargetEnemy();
	UFUNCTION(BlueprintCallable)
	void ResetState();

	void SwitchMovementStyle(EMovementStyle MovementStyle);
	void SwitchCameraMovement(ECameraMovement CameraMovement);
	void LookAtTarget();

	void LookAtInventory();

	UFUNCTION(BlueprintCallable)
	void EquipWeaponR(int32 Index);
	UFUNCTION(BlueprintCallable)
	void EquipWeaponL(int32 Index);

	void QuickSortUsables(TArray<AUsable*> Inventory, int32 Low, int32 High);
	int32 Partition(TArray<AUsable*> Inventory, int32 Low, int32 High);
	void Swap(int32 i, int32 j);

	void DutchQuickSort(TArray<AUsable*> Inventory, int Left, int Right);
	void DutchPartition(TArray<AUsable*> Arr, int Left, int Right, int i, int j);

	UFUNCTION(BlueprintCallable)
	bool RemoveAndSetIngredient(int32 ResourceStackIndex, int32 ResourceSelectIndex, UTexture2D*& ResourceImage);
	UFUNCTION(BlueprintCallable)
	void ResetCraftingIngredients(bool ResetFirst, bool ResetSecond);

	UFUNCTION(BlueprintCallable)
	void GetResource(int32 ResourceStackIndex, int32 InUseIngredientIndex, int32& ResourceInventoryIndex, UTexture2D*& ResourceImage, bool& bHasResource);
	UFUNCTION(BlueprintCallable)
	void GetResourceImage(int32 ResourceStackIndex, UTexture2D*& ResourceImage);
	UFUNCTION(BlueprintCallable)
	void GetResourceCount(int32 ResourceStackIndex, int32& ResourceCount);

	UFUNCTION(BlueprintCallable)
	UTexture2D* CheckCanCraft();
	UFUNCTION(BlueprintCallable)
	bool AddUsable();
	UFUNCTION(BlueprintCallable)
	bool CheckCanCraftMore();
	UFUNCTION(BlueprintCallable)
	void ResetIngredients();
	UFUNCTION(BlueprintCallable)
	int32 GetIngredientsCount(bool First, bool Second);
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetIngredientImage(bool First, bool Second);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetEquipmentImage(int32 Index);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetGearImage(int32 Index, int32& ItemCount);

	void GetGear(int32 Index);
	UFUNCTION(BlueprintCallable)
	void UseDesiredGear();

	void SetAttackModifier(int32 Time, float Modifier);
	void ResetAttackModifier();
	UFUNCTION(BlueprintCallable)
	float GetAttackTimeRemaining();

	void SetDefenseModifier(int32 Time, float Modifier);
	void ResetDefenseModifier();
	UFUNCTION(BlueprintCallable)
	float GetDefenseTimeRemaining();

	void SetMobilityModifier(int32 Time, float Modifier);
	void ResetMobilityModifier();
	UFUNCTION(BlueprintCallable)
	float GetMobilityTimeRemaining();

	void Heal(float Amount);

	void PlayUseMontage(FName MontageSection);

	UFUNCTION(BlueprintCallable) void SetGearUseIndex(int32 GearBoxIndex, int32 DesiredInventoryIndex);
	UFUNCTION(BlueprintCallable) void GetGearStack(int32 StackIndex, int32 SlotIndex);
	UFUNCTION(BlueprintCallable) void RemoveGearStack(int32 SlotIndex);
	void AddToGearSlot(AUsable* UsableToAdd, int32 SlotIndex);
	UFUNCTION(BlueprintCallable) void SwapGearSlot(int32 FirstSlot, int32 SecondSlot);
	UFUNCTION(BlueprintCallable) void GetGearSlotImageAndCount(int32 SlotIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear);
	UFUNCTION(BlueprintCallable) void GetGearInventoryStackImageAndCount(int32 StackIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear);

	void Dodge();
	void Block();
	virtual void Stun() override;

	void SetStaminaRechargeTimer(float RechargeDelay);
	void ResetStaminaRecharge();
	void UseStamina(float StaminaCost, float RechargeDelay);

	UFUNCTION(BlueprintCallable)
	void ResetDodge();

	UFUNCTION()
	void EnemyDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void EnemyDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void DeathEnd() override;

	UFUNCTION(BlueprintCallable)
	void OpenCombo();

	UFUNCTION(BlueprintCallable)
	void ResetCombo();

	void Q();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void LMB();
	void RMBPressed();
	void RMBReleased();
	void MMB();
	void E();
	
	void OnePressed();
	void TwoPressed();
	void ThreePressed();
	void FourPressed();
};
