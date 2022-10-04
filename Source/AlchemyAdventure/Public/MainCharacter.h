// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSetImageAndCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastOpenCharacterMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastResetInventoryHUD);

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
	TArray<APickup*> OverlappingPickups;
	APickup* CurrentPickup;
	bool bCanPickup = false;
	int32 PickupsCount = 0;

	int32 EquippedWeaponIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32>CurrentGearIndexes{ -1, -1, -1, -1 };

	float MaxWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxLockOnDistance = 2000.f;
	bool bFaceTarget = true;
	bool bCanDodge = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bDodging = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bInvincible = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bBlocking = false;

	bool bInventoryOpen = false;
	FRotator CameraFixedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stamina = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaRechargeRate = 25.f;

	bool bStaminaCanRecharge = true;

	FTimerHandle ResetStaminaRechargeTimer;

	//Status Effects
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

	FName IngredientOneName;
	FName IngredientTwoName;

	//Delegates
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastSetImageAndCount DynamicMulticastSetImageAndCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastResetInventoryHUD DynamicMulticastResetInventoryHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable)
	FDynamicMulticastOpenCharacterMenu DynamicMulticastOpenCharacterMenu;

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
	FORCEINLINE void AddXP(int32 AddedXP) { XP += AddedXP; }

	void CheckOverlappingPickups();
	void SetPickups(APickup* Pickup, bool Overlapped);

	bool FindBestTargetEnemy();
	void LockOnEnemy();
	void UntargetEnemy();

	void SwitchMovementStyle(EMovementStyle MovementStyle);
	void SwitchCameraMovement(ECameraMovement CameraMovement);
	void LookAtTarget();

	void LookAtInventory();

	UFUNCTION(BlueprintCallable)
	void EquipWeaponR(int32 Index);
	UFUNCTION(BlueprintCallable)
	void EquipWeaponL(int32 Index);

	UFUNCTION(BlueprintCallable)
	void GetResource(int32 ResourceStackIndex, int32 InUseIngredientIndex, int32& ResourceInventoryIndex, UTexture2D*& ResourceImage, bool& bHasResource);
	UFUNCTION(BlueprintCallable)
	void GetResourceImage(int32 ResourceStackIndex, UTexture2D*& ResourceImage);
	UFUNCTION(BlueprintCallable)
	void GetResourceCount(int32 ResourceStackIndex, int32& ResourceCount);

	UFUNCTION(BlueprintCallable)
	UTexture2D* CheckCanCraft(int32 FirstIndex, int32 SecondIndex);
	UFUNCTION(BlueprintCallable)
	bool AddUsable(int32 FirstIngredient, int32 SecondIngredient);
	UFUNCTION(BlueprintCallable)
	bool CheckCanCraftMore(int32 FirstIngredientIndex, int32 SecondIngredientIndex);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetEquipmentImage(int32 Index);

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetGearImage(int32 Index, int32& ItemCount);

	void GetGear(int32 Index);
	void SetAttackModifier(int32 Time, float Modifier);
	void ResetAttackModifier();
	void SetDefenseModifier(int32 Time, float Modifier);
	void ResetDefenseModifier();
	void SetMobilityModifier(int32 Time, float Modifier);
	void ResetMobilityModifier();
	void Heal(float Amount);

	void PlayUseMontage(FName MontageSection);

	UFUNCTION(BlueprintCallable)
	void SetGearUseIndex(int32 GearBoxIndex, int32 DesiredInventoryIndex);

	void SortInventory();
	void QuickSort(TArray<TSubclassOf<AItem>> Inventory, int Left, int Right);
	void Partition(TArray<TSubclassOf<AItem>> Arr, int Left, int Right, int i, int j);
	void Swap(int Left, int Right);

	void Dodge();

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
	void Q();
	void OnePressed();
	void TwoPressed();
	void ThreePressed();
	void FourPressed();
};
