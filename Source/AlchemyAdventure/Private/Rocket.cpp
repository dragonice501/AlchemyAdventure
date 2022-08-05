// Fill out your copyright notice in the Description page of Project Settings.


#include "Rocket.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Controller.h"

// Sets default values
ARocket::ARocket()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetSimulatePhysics(true);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARocket::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARocket::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Spacebar", this, &ARocket::Throttle);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARocket::Turn);
}

void ARocket::Throttle(float Value)
{
	if (BoxComponent && Value != 0.f)
	{
		FVector Up = BoxComponent->GetUpVector();
		BoxComponent->AddForce(Up * 1500.f * Value, FName(), true);
	}
}

void ARocket::Turn(float Value)
{
	if (BoxComponent && Value != 0.f)
	{
		FQuat DeltaRotation{ 1.f * GetWorld()->GetDeltaSeconds() * -Value, 0.f, 0.f, 1.f };
		//GetOwner()->AddActorLocalRotation(DeltaRotation);
		BoxComponent->AddLocalRotation(DeltaRotation);
	}
}