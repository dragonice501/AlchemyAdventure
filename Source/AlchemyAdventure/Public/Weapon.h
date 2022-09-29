// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Sword UMETA(DisplayName = "Sword"),
	EWT_Spear UMETA(DisplayName = "Spear"),
};

UCLASS()
class ALCHEMYADVENTURE_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UArrowComponent* ArrowComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;

	class ABaseCharacter* Owner;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UDataTable* WeaponPropertiesDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName DataTableRow;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float StaminRechargeDelay;
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	void BuildWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ActivateCollision();
	void DeactivateCollision();

	TArray<class ABaseCharacter*> HitCharacters;

	FORCEINLINE ABaseCharacter* GetCharacter() const { return Owner; }

	FORCEINLINE void SetMainCharacter(ABaseCharacter* Character) { Owner = Character; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
