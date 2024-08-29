// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSetImageAndCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastOpenCharacterMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastResetInventoryHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetAttackTimer, bool, bToggleAttackTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetDefenseTimer, bool, bToggleDefenseTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetMobilityTimer, bool, bToggleMobilityTimer);

class UInputMappingContext;
class UInputAction;
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
class ALCHEMYADVENTURE_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SpringArmDefaultLength = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USphereComponent* EnemyDetectionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) AMainPlayerController* MainPlayerController;

	// Input
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputMappingContext* mMappingContext;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mMoveAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mLookAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mAttackAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mStartBlockAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mEndBlockAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mDodgeAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mInteractAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mMenuAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mToggleLockOnAction;
	
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mOneShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mTwoShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mThreeShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Input") UInputAction* mFourShortcutAction;

	// Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* DodgeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* UseMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* GuardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* HurtMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims") UAnimMontage* RecoilMontage;

	// Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float Poise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float MaxPoise;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") bool bIsDead = false;
	FTimerHandle ResetPoiseRechargeTimer;

	FVector InputVector;

	AEnemy* TargetEnemy = nullptr;
	FVector EnemyLockOnPosition;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") float RotatingActorRotate = 180.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") bool bLockedOn = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") bool bUsing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") bool bCanCombo = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat") int32 ComboCount = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") bool bDodging = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") bool bInvincible = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat") bool bBlocking = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") bool bAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat") float StunChance;
	bool bStunned = false;

	// Inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AWeapon* RightHandEquipment = nullptr;
	int32 RighHandIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	AWeapon* LeftHandEquipment = nullptr;
	int32 LeftHandIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AWeapon> StartingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<TSubclassOf<AWeapon>> EquipmentInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AResource*> ResourceInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AUsable*> UsablesInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<TSubclassOf<AUsable>> StartingUsables;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<TSubclassOf<AUsable>> PotionMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<APickup*> OverlappingPickups;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AItem>PickupItem;
	APickup* CurrentPickup;
	bool bCanPickup = false;
	int32 PickupsCount = 0;

	int32 EquippedWeaponIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32>CurrentGearIndexes{ -1, -1, -1, -1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AUsable*> GearSlotOneInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AUsable*> GearSlotTwoInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AUsable*> GearSlotThreeInventory;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory") TArray<AUsable*> GearSlotFourInventory;

	// Movement
	float MaxWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxLockOnDistance = 2000.f;
	bool bFaceTarget = true;
	bool bCanDodge = false;

	bool bInventoryOpen = false;
	FRotator CameraFixedRotation;

	// Character Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float Stamina = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes") float StaminaRechargeRate = 25.f;
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

	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsOneInv;
	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsTwoInv;

	//Delegates
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastSetImageAndCount DynamicMulticastSetImageAndCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastResetInventoryHUD DynamicMulticastResetInventoryHUD;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastOpenCharacterMenu DynamicMulticastOpenCharacterMenu;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastSetAttackTimer DynamicMulticastSetAttackTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastSetDefenseTimer DynamicMulticastSetDefenseTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastSetMobilityTimer DynamicMulticastSetMobilityTimer;

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Overrides
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	// Input
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void Interact(const FInputActionValue& value);
	void Menu(const FInputActionValue& value);
	void ToggleLockOn(const FInputActionValue& value);
	void StartBlock(const FInputActionValue& value);
	void EndBlock(const FInputActionValue& value);
	
	void OnePressed(const FInputActionValue& value);
	void TwoPressed(const FInputActionValue& value);
	void ThreePressed(const FInputActionValue& value);
	void FourPressed(const FInputActionValue& value);

	// Animation

	// Base Character
	void DepletePoise(float Cost);
	void SetPoiseRechargeTimer();
	void ResetPoise();

	virtual void Stagger();
	void Recoil();

	UFUNCTION(BlueprintCallable) void ResetStunned();

	virtual void Die(AActor* Causer);
	UFUNCTION(BlueprintCallable) virtual void DeathEnd();

	UFUNCTION(BlueprintCallable) void EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip);
	UFUNCTION(BlueprintCallable) void EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip);

	UFUNCTION(BlueprintCallable) void ActivateWeapon();
	UFUNCTION(BlueprintCallable) void DeactivateWeapon();

	// Attributes
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

	UFUNCTION(BlueprintCallable) void EquipWeaponR(int32 Index);
	UFUNCTION(BlueprintCallable) void EquipWeaponL(int32 Index);

	void QuickSortUsables(TArray<AUsable*> Inventory, int32 Low, int32 High);
	int32 Partition(TArray<AUsable*> Inventory, int32 Low, int32 High);
	void Swap(int32 i, int32 j);
	void DutchQuickSort(TArray<AUsable*> Inventory, int Left, int Right);
	void DutchPartition(TArray<AUsable*> Arr, int Left, int Right, int i, int j);

	UFUNCTION(BlueprintCallable) bool RemoveAndSetIngredient(int32 ResourceStackIndex, int32 ResourceSelectIndex, UTexture2D*& ResourceImage);
	UFUNCTION(BlueprintCallable) void ResetCraftingIngredients(bool ResetFirst, bool ResetSecond);

	UFUNCTION(BlueprintCallable) void GetResource(int32 ResourceStackIndex, int32 InUseIngredientIndex, int32& ResourceInventoryIndex, UTexture2D*& ResourceImage, bool& bHasResource);
	UFUNCTION(BlueprintCallable) void GetResourceImage(int32 ResourceStackIndex, UTexture2D*& ResourceImage);
	UFUNCTION(BlueprintCallable) void GetResourceCount(int32 ResourceStackIndex, int32& ResourceCount);

	UFUNCTION(BlueprintCallable) UTexture2D* CheckCanCraft();
	UFUNCTION(BlueprintCallable) bool AddUsable();
	UFUNCTION(BlueprintCallable) bool CheckCanCraftMore();
	UFUNCTION(BlueprintCallable) int32 GetIngredientsCount(bool First, bool Second);
	UFUNCTION(BlueprintCallable) UTexture2D* GetIngredientImage(bool First, bool Second);

	UFUNCTION(BlueprintCallable) UTexture2D* GetEquipmentImage(int32 Index);
	UFUNCTION(BlueprintCallable) UTexture2D* GetGearImage(int32 Index, int32& ItemCount);

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
	UFUNCTION(BlueprintCallable) void SwapGearSlot(int32 FirstSlot, int32 SecondSlot);
	UFUNCTION(BlueprintCallable) void GetGearSlotImageAndCount(int32 SlotIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear);
	UFUNCTION(BlueprintCallable) void GetGearInventoryStackImageAndCount(int32 StackIndex, UTexture2D*& OutImage, int32& Count, bool& HasGear);
	void AddToGearSlot(AUsable* UsableToAdd, int32 SlotIndex);

	void Dodge();
	void Block();

	void SetStaminaRechargeTimer(float RechargeDelay);
	void ResetStaminaRecharge();
	void UseStamina(float StaminaCost, float RechargeDelay);

	UFUNCTION(BlueprintCallable)
	void ResetDodge();

	UFUNCTION()
	void EnemyDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EnemyDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable) void OpenCombo();
	UFUNCTION(BlueprintCallable) void ResetCombo();
};
