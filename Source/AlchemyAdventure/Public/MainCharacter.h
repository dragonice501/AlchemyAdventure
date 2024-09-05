// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAttributesComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSetImageAndCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastOpenCharacterMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastResetInventoryHUD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastPickupItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastUseGear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetAttackTimer, bool, bToggleAttackTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetDefenseTimer, bool, bToggleDefenseTimer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastSetMobilityTimer, bool, bToggleMobilityTimer);

class AEnemy;
class AItem;
class AMainPlayerController;
class APickup;
class AResource;
class AUsable;
class UCameraComponent;
class UCharacterAttributesComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerInventoryComponent;
class USpringArmComponent;
class USphereComponent;

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
	// Controller
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) AMainPlayerController* MainPlayerController;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float SpringArmDefaultLength = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) USphereComponent* EnemyDetectionSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UCharacterAttributesComponent* mAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UPlayerInventoryComponent* mInventory;

	// Input
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputMappingContext* mMappingContext;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mMoveAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mLookAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mAttackAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mStartBlockAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mEndBlockAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mDodgeAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mInteractAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mMenuAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mToggleLockOnAction;														   
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mOneShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mTwoShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mThreeShortcutAction;
	UPROPERTY(Editanywhere, BlueprintReadOnly, Category = "Player Character | Input") UInputAction* mFourShortcutAction;

	// Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* DodgeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* UseMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* GuardMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* HurtMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Character | Animation") UAnimMontage* RecoilMontage;

	FVector mInputVector;

	AEnemy* TargetEnemy = nullptr;
	FVector EnemyLockOnPosition;

	// Combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") USoundBase* HitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") float RotatingActorRotate = 180.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character | Combat") bool bLockedOn = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") bool bUsing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") bool bCanCombo = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") int32 ComboCount = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character | Combat") bool bDodging = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") bool bInvincible = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character | Combat") bool bBlocking = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") bool bAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Character | Combat") float StunChance;
	bool bStunned = false;

	// Equipment
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character | Equipment") AWeapon* rightHandEquipment = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Character | Equipment") AWeapon* leftHandEquipment = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Character | Equipment") TSubclassOf<AWeapon> StartingWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32>CurrentGearIndexes{ -1, -1, -1, -1 };
	int32 RighHandIndex = -1;
	int32 LeftHandIndex = -1;
	int32 EquippedWeaponIndex = 0;

	// Inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<APickup*> OverlappingPickups;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AItem>PickupItem;
	APickup* currentPickup;
	bool bCanPickup = false;
	int32 PickupsCount = 0;

	// Movement
	float MaxWalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float MaxLockOnDistance = 2000.f;
	bool bFaceTarget = true;
	bool bCanDodge = false;

	bool bInventoryOpen = false;
	FRotator CameraFixedRotation;

	//Status Effects
	AUsable* DesiredGearToUse = nullptr;
	int32 mDesiredGearSlot = -1;
	int32 StatusEffectTime;

	FTimerHandle attackModifierTimer;
	bool bAttackModifier = false;
	float attackModifier = 1.f;

	FTimerHandle defenseModifierTimer;
	bool bDefenseModifier = false;
	float defenseModifier = 1.f;

	FTimerHandle mobilityModifierTimer;
	bool bMobilityModifier = false;
	float mobilityModifier = 1.f;

	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsOneInv;
	UPROPERTY(VisibleAnywhere) TArray<AResource*> SetIngredientsTwoInv;

	//Delegates
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastPickupItem DynamicMulticastPickupItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintAssignable) FDynamicMulticastUseGear DynamicMulticastUseGear;
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
	UFUNCTION() void EnemyDetectionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void EnemyDetectionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Input
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Menu(const FInputActionValue& value);
	void Interact(const FInputActionValue& value);
	void ToggleLockOn(const FInputActionValue& value);
	void Attack(const FInputActionValue& value);
	void StartBlock(const FInputActionValue& value);
	void EndBlock(const FInputActionValue& value);
	void Dodge(const FInputActionValue& value);
	
	void OnePressed(const FInputActionValue& value);
	void TwoPressed(const FInputActionValue& value);
	void ThreePressed(const FInputActionValue& value);
	void FourPressed(const FInputActionValue& value);

	// Base Character
	virtual void Die(AActor* Causer);
	UFUNCTION(BlueprintCallable) virtual void DeathEnd();

	UFUNCTION(BlueprintCallable) void EquipRightHand(TSubclassOf<AWeapon> WeaponToEquip);
	UFUNCTION(BlueprintCallable) void EquipLeftHand(TSubclassOf<AWeapon> WeaponToEquip);

	UFUNCTION(BlueprintCallable) void ActivateWeapon();
	UFUNCTION(BlueprintCallable) void DeactivateWeapon();

	// Interacting
	void CheckOverlappingPickups();
	void SetPickups(APickup* Pickup, bool Overlapped);

	bool FindBestTargetEnemy();
	void LockOnEnemy();
	void UntargetEnemy();
	UFUNCTION(BlueprintCallable)void ResetState();

	void SwitchMovementStyle(EMovementStyle MovementStyle);
	void SwitchCameraMovement(ECameraMovement CameraMovement);
	void LookAtTarget();

	// Equipment
	UFUNCTION(BlueprintCallable) void EquipWeaponR(int32 index);
	UFUNCTION(BlueprintCallable) void EquipWeaponL(int32 index);
	UFUNCTION(BlueprintCallable) void UnequipWeaponR();

	UFUNCTION(BlueprintCallable) UTexture2D* GetRightWeaponImage();
	
	// Inventory
	void LookAtInventory();

	// Gear
	UFUNCTION(BlueprintCallable)void UseGearFromInventory();

	// Modifiers
	UFUNCTION(BlueprintCallable) float GetAttackTimeRemaining();
	UFUNCTION(BlueprintCallable) float GetDefenseTimeRemaining();
	UFUNCTION(BlueprintCallable) float GetMobilityTimeRemaining();
	void SetAttackModifier(int32 Time, float Modifier);
	void SetDefenseModifier(int32 Time, float Modifier);
	void SetMobilityModifier(int32 Time, float Modifier);
	void UseGear(int32 gearSlotIndex);
	void ResetAttackModifier();
	void ResetDefenseModifier();
	void ResetMobilityModifier();

	void Heal(float Amount);

	void PlayUseMontage(FName MontageSection);

	// Combat
	void Stagger();
	void Recoil();
	void BlockHit();

	UFUNCTION(BlueprintCallable) void OpenCombo();
	UFUNCTION(BlueprintCallable) void ResetCombo();
	UFUNCTION(BlueprintCallable) void ResetDodge();
	UFUNCTION(BlueprintCallable) void ResetStunned();

	bool Unoccupied();
};
