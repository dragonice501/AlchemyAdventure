// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
		HUDOverlay->AddToViewport();
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (WLockOnWidget)
	{
		LockOnWidget = CreateWidget<UUserWidget>(this, WLockOnWidget);
		if (LockOnWidget)
		{
			LockOnWidget->AddToViewport();
			LockOnWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WPickupPromptWidget)
	{
		PickupPromptWidget = CreateWidget<UUserWidget>(this, WPickupPromptWidget);
		if (PickupPromptWidget)
		{
			PickupPromptWidget->AddToViewport();
			PickupPromptWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WPlayerInventoryWidget)
	{
		PlayerInventoryWidget = CreateWidget<UUserWidget>(this, WPlayerInventoryWidget);
		if (PlayerInventoryWidget)
		{
			PlayerInventoryWidget->AddToViewport();
			PlayerInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
			//PlayerInventoryWidget->
		}
	}

	if (WPlayerStatsWidget)
	{
		PlayerStatsWidget = CreateWidget<UUserWidget>(this, WPlayerStatsWidget);
		if (PlayerStatsWidget)
		{
			PlayerStatsWidget->AddToViewport();
			PlayerStatsWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	
}

void AMainPlayerController::ToggleInventory(bool Visible)
{
	if (Visible)
	{
		bInventoryVisible = true;
		PlayerInventoryWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);

		SetShowMouseCursor(true);
		FVector2D ScreenSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ScreenSize);
		}

		SetMouseLocation(ScreenSize.X * 0.5f, ScreenSize.Y * 0.5f);
	}
	else
	{
		bInventoryVisible = false;
		PlayerInventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
	}
}

void AMainPlayerController::DisplayLockOn(FVector Position)
{
	if (LockOnWidget)
	{
		if (LockOnWidget->GetVisibility() == ESlateVisibility::Hidden)
		{
			LockOnWidget->SetVisibility(ESlateVisibility::Visible);
		}

		FVector2D EnemyScreenPosition;
		bool bEnemyScreenToWorld = UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this, 0), Position, EnemyScreenPosition);

		if (bEnemyScreenToWorld)
		{
			LockOnWidget->SetPositionInViewport(EnemyScreenPosition);
		}
	}
}

void AMainPlayerController::RemoveLockOn()
{
	if (LockOnWidget)
	{
		LockOnWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayPickupPrompt(FString PromptText)
{
	if (PickupPromptWidget)
	{
		PickupPromptText = PromptText;
		PickupPromptWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemovePickupPrompt()
{
	if (PickupPromptWidget)
	{
		PickupPromptWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::DisplayStats()
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveStats()
{
	if (PlayerStatsWidget)
	{
		PlayerStatsWidget->SetVisibility(ESlateVisibility::Visible);
	}
}