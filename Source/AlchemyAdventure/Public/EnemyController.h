// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ALCHEMYADVENTURE_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Behavior")
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Behavior")
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	class AEnemy* ControlledEnemy;
	
public:

	AEnemyController();

	virtual void OnPossess(APawn* InPawn) override;

	FORCEINLINE UBlackboardComponent* GetBlackboard() const { return BlackboardComponent; }

	UFUNCTION(BlueprintCallable)
	void ResetState();

	void EndBehaviorTree();
};
