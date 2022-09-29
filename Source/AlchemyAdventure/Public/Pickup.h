// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Pickup.generated.h"

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EPT_Resource UMETA(DisplayName = "Resource"),
	EPT_Item UMETA(DisplayName = "Item"),
	EPT_Drop UMETA(DisplayName = "Drop"),
};

class AResource;

UCLASS()
class ALCHEMYADVENTURE_API APickup : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystemComponent* IdleParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPickupType PickupType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AItem> BPPickupItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	
public:	
	// Sets default values for this actor's properties
	APickup();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FString GetPickupPrompt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
