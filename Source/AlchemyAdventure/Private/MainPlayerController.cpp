// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "CharacterAttributesComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AlchemyAdventureSaveGame.h"
#include "MainCharacter.h"
#include "Weapon.h"
#include "Usable.h"

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

void AMainPlayerController::SaveGame()
{
	UAlchemyAdventureSaveGame* SaveGameInstance = Cast<UAlchemyAdventureSaveGame>(UGameplayStatics::CreateSaveGameObject(UAlchemyAdventureSaveGame::StaticClass()));
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetCharacter());

	if (SaveGameInstance && MainCharacter && MainCharacter->mAttributes)
	{
		SaveGameInstance->LevelToLoad = UGameplayStatics::GetCurrentLevelName(this);
		SaveGameInstance->PlayerLocation = MainCharacter->GetActorLocation();
		SaveGameInstance->PlayerRotation = MainCharacter->GetActorRotation();
		SaveGameInstance->Health = MainCharacter->mAttributes->Health;
		SaveGameInstance->EquipmentInventory = MainCharacter->EquipmentInventory;
		SaveGameInstance->RightHandIndex = MainCharacter->RighHandIndex;
		SaveGameInstance->ResourceInventory = MainCharacter->ResourceInventory;
		SaveGameInstance->UsablesInventory = MainCharacter->UsablesInventory;
		SaveGameInstance->GearSlotOneInventory = MainCharacter->GearSlotOneInventory;
		SaveGameInstance->GearSlotTwoInventory = MainCharacter->GearSlotTwoInventory;
		SaveGameInstance->GearSlotThreeInventory = MainCharacter->GearSlotThreeInventory;
		SaveGameInstance->GearSlotFourInventory = MainCharacter->GearSlotFourInventory;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserSlot);
	}
}

void AMainPlayerController::LoadGame()
{
	UAlchemyAdventureSaveGame* LoadGameInstance = Cast<UAlchemyAdventureSaveGame>(UGameplayStatics::CreateSaveGameObject(UAlchemyAdventureSaveGame::StaticClass()));
	LoadGameInstance = Cast<UAlchemyAdventureSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserSlot));
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetCharacter());

	if (LoadGameInstance && MainCharacter && MainCharacter->mAttributes)
	{
		MainCharacter->SetActorLocation(LoadGameInstance->PlayerLocation);
		MainCharacter->SetActorRotation(LoadGameInstance->PlayerRotation);

		MainCharacter->mAttributes->Health = LoadGameInstance->Health;
		MainCharacter->EquipmentInventory = LoadGameInstance->EquipmentInventory;
		MainCharacter->RighHandIndex = LoadGameInstance->RightHandIndex;
		if (MainCharacter->RighHandIndex != -1) MainCharacter->EquipWeaponR(MainCharacter->RighHandIndex);
		MainCharacter->ResourceInventory = LoadGameInstance->ResourceInventory;
		MainCharacter->UsablesInventory = LoadGameInstance->UsablesInventory;
		MainCharacter->GearSlotOneInventory = LoadGameInstance->GearSlotOneInventory;
		MainCharacter->GearSlotTwoInventory = LoadGameInstance->GearSlotTwoInventory;
		MainCharacter->GearSlotThreeInventory = LoadGameInstance->GearSlotThreeInventory;
		MainCharacter->GearSlotFourInventory = LoadGameInstance->GearSlotFourInventory;
	}
}
