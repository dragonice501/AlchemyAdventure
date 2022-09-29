// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "MainCharacter.h"
#include "Enemy.h"

AEnemyController::AEnemyController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		ControlledEnemy = Cast<AEnemy>(InPawn);
		if (ControlledEnemy)
		{
			if (ControlledEnemy->BehaviorTree)
			{
				BlackboardComponent->InitializeBlackboard(*(ControlledEnemy->BehaviorTree->BlackboardAsset));
				BehaviorTreeComponent->StartTree(*ControlledEnemy->BehaviorTree);
				BlackboardComponent->SetValueAsVector(TEXT("InitialLocation"), ControlledEnemy->GetActorLocation());
			}
		}
	}
}

void AEnemyController::ResetState()
{
	BlackboardComponent->ClearValue(TEXT("TargetLastLocation"));
}

void AEnemyController::EndBehaviorTree()
{
	BehaviorTreeComponent->StopTree();
}