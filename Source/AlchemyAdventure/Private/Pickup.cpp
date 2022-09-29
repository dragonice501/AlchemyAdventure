// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMesh.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Item.h"
#include "Resource.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	SetRootComponent(OverlapSphere);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	IdleParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IdleParticles"));
	IdleParticles->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::BeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &APickup::EndOverlap);
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetPickups(this, true);
		}
	}
}

void APickup::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MainCharacter->SetPickups(this, false);
		}
	}
}

FString APickup::GetPickupPrompt()
{
	FString Prompt = "";
	switch (PickupType)
	{
	case EPickupType::EPT_Resource:
		Prompt = "Gather Resource";
		break;
	case EPickupType::EPT_Item:
		Prompt = "Pickup Item";
		break;
	default:
		break;
	}

	return Prompt;
}