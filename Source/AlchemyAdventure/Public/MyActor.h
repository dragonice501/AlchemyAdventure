// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class ALCHEMYADVENTURE_API AMyActor : public AActor
{
	GENERATED_BODY()

private:
	float mRunningTime;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* mMesh;
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
