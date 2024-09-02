// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "Usable.h"
#include "Resource.h"
#include "Weapon.h"

// Sets default values for this component's properties
UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UPlayerInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerInventoryComponent::AddToResourceInventory(const FDataTableRowHandle& itemName)
{
	FResourcePropertyTable* row = itemName.GetRow<FResourcePropertyTable>(itemName.RowName.ToString());

	for (TPair<FR, uint8>& resource : resourceMap)
	{
		if (row->resourceName == resource.Key.resourceName)
		{
			resource.Value++;
			return;
		}
	}

	FR newResource;
	newResource.BuildResource(row);
	resourceMap.Add(newResource, 1);
}

void UPlayerInventoryComponent::AddToWeaponInventory(TSubclassOf<AWeapon> weapon)
{
	weaponInventory.Add(weapon);
}

bool UPlayerInventoryComponent::RemoveAndSetIngredient(int32 resourceStackIndex, int32 resourceSelectIndex, UTexture2D*& resourceImage)
{
	int32 Index = 0;
	int32 resourceIndex = 0;

	if (resourceInventory.Num() > 0)
	{
		AResource* currentResource = nullptr;
		AResource* previousResource = nullptr;

		for (int i = 0; i < resourceInventory.Num(); i++)
		{
			resourceIndex = i;
			if (i == 0) previousResource = resourceInventory[0];
			else previousResource = currentResource;
			currentResource = resourceInventory[i];

			if (currentResource->resourceName == previousResource->resourceName)
			{
				if (Index == resourceStackIndex)
				{
					resourceImage = currentResource->resourceImage;
					if (resourceSelectIndex == 0)
					{
						if (setIngredientsOneInv.Num() > 0)
						{
							if (setIngredientsOneInv[0]->resourceName != currentResource->resourceName)
							{
								while (setIngredientsOneInv.Num() > 0)
								{
									resourceInventory.Add(setIngredientsOneInv[0]);
									setIngredientsOneInv.RemoveAt(0);
								}
								setIngredientsOneInv.Add(currentResource);
								resourceInventory.RemoveAt(i);
								//SortResourceInventory();
							}
							else
							{
								setIngredientsOneInv.Add(currentResource);
								resourceInventory.RemoveAt(resourceIndex);
							}
						}
						else
						{
							setIngredientsOneInv.Add(currentResource);
							resourceInventory.RemoveAt(resourceIndex);
						}

						return true;
					}
					else if (resourceSelectIndex == 1)
					{
						if (setIngredientsTwoInv.Num() > 0)
						{
							if (setIngredientsTwoInv[0]->resourceName != currentResource->resourceName)
							{
								while (setIngredientsTwoInv.Num() > 0)
								{
									resourceInventory.Add(setIngredientsTwoInv[0]);
									setIngredientsTwoInv.RemoveAt(0);
								}
								setIngredientsTwoInv.Add(currentResource);
								resourceInventory.RemoveAt(i);
								//SortResourceInventory();
							}
							else
							{
								setIngredientsTwoInv.Add(currentResource);
								resourceInventory.RemoveAt(resourceIndex);
							}
						}
						else
						{
							setIngredientsTwoInv.Add(currentResource);
							resourceInventory.RemoveAt(resourceIndex);
						}

						return true;
					}
				}
			}
			else if (currentResource->resourceName != previousResource->resourceName)
			{
				Index++;
				if (Index == resourceStackIndex)
				{
					resourceImage = currentResource->resourceImage;
					if (resourceSelectIndex == 0)
					{
						if (setIngredientsOneInv.Num() > 0)
						{
							if (setIngredientsOneInv[0]->resourceName != currentResource->resourceName)
							{
								while (setIngredientsOneInv.Num() > 0)
								{
									resourceInventory.Add(setIngredientsOneInv[0]);
									setIngredientsOneInv.RemoveAt(0);
								}
								setIngredientsOneInv.Add(currentResource);
								resourceInventory.RemoveAt(i);
								//SortResourceInventory();
							}
							else
							{
								setIngredientsOneInv.Add(currentResource);
								resourceInventory.RemoveAt(resourceIndex);
							}
						}
						else
						{
							setIngredientsOneInv.Add(currentResource);
							resourceInventory.RemoveAt(resourceIndex);
						}

						return true;
					}
					else if (resourceSelectIndex == 1)
					{
						if (setIngredientsTwoInv.Num() > 0)
						{
							if (setIngredientsTwoInv[0]->resourceName != currentResource->resourceName)
							{
								while (setIngredientsTwoInv.Num() > 0)
								{
									resourceInventory.Add(setIngredientsTwoInv[0]);
									setIngredientsTwoInv.RemoveAt(0);
								}
								setIngredientsTwoInv.Add(currentResource);
								resourceInventory.RemoveAt(i);
								//SortResourceInventory();
							}
							else
							{
								setIngredientsTwoInv.Add(currentResource);
								resourceInventory.RemoveAt(resourceIndex);
							}
						}
						else
						{
							setIngredientsTwoInv.Add(currentResource);
							resourceInventory.RemoveAt(resourceIndex);
						}

						return true;
					}
				}
			}
		}
	}

	return false;
}

void UPlayerInventoryComponent::ResetCraftingIngredients(bool resetFirst, bool resetSecond)
{
	if (setIngredientsOneInv.Num() > 0 && resetFirst)
	{
		while (setIngredientsOneInv.Num() > 0)
		{
			resourceInventory.Add(setIngredientsOneInv[0]);
			setIngredientsOneInv.RemoveAt(0);
		}
	}

	if (setIngredientsTwoInv.Num() > 0 && resetSecond)
	{
		while (setIngredientsTwoInv.Num() > 0)
		{
			resourceInventory.Add(setIngredientsTwoInv[0]);
			setIngredientsTwoInv.RemoveAt(0);
		}
	}

	//SortResourceInventory();
}

void UPlayerInventoryComponent::GetResource(int32 resourceStackIndex, int32 inUseIngredientIndex, int32& resourceInventoryIndex, UTexture2D*& resourceImage, bool& hasResource)
{
	int32 Index = 0;

	if (resourceInventory.Num() > 0)
	{
		AResource* currentResource = nullptr;
		AResource* previousResource = nullptr;

		for (int i = 0; i < resourceInventory.Num(); i++)
		{
			resourceInventoryIndex = i;
			if (i == 0) previousResource = resourceInventory[0];
			else previousResource = currentResource;
			currentResource = resourceInventory[i];

			if (currentResource->resourceName != previousResource->resourceName)
			{
				if (Index == resourceStackIndex)
				{
					if (i - 1 != inUseIngredientIndex)
					{
						resourceImage = previousResource->resourceImage;
						resourceInventoryIndex = i - 1;
						hasResource = true;
						return;
					}
					/*else if (i != inUseIngredientIndex)
					{
						UE_LOG(LogTemp, Warning, TEXT("new previous"));
						resourceImage = PreviousResource->resourceImage;
						resourceInventoryIndex = i - 1;
						hasResource = true;
						return;
					}*/
				}
				Index++;
			}
			else if (currentResource->resourceName == previousResource->resourceName)
			{
				if (Index == resourceStackIndex)
				{
					if (i != inUseIngredientIndex)
					{
						resourceImage = previousResource->resourceImage;
						resourceInventoryIndex = i;
						hasResource = true;
						return;
					}
					else if (i - 1 != inUseIngredientIndex && resourceInventory[i]->resourceName == currentResource->resourceName)
					{
						resourceImage = previousResource->resourceImage;
						resourceInventoryIndex = i - 1;
						hasResource = true;
						return;
					}
				}
			}
		}

		if (Index == resourceStackIndex && resourceInventoryIndex != inUseIngredientIndex)
		{
			resourceImage = currentResource->resourceImage;
			hasResource = true;
			return;
		}
	}

	hasResource = false;
}

void UPlayerInventoryComponent::GetResourceImage(int32 resourceStackIndex, UTexture2D*& resourceImage)
{
	int32 index = 0;
	for (TPair<FR, uint8>& resource : resourceMap)
	{
		if (index == resourceStackIndex)
		{
			resourceImage = resource.Key.resourceImage;
			return;
		}

		index++;
	}

	/*if (resourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < resourceInventory.Num(); i++)
		{
			if (i == 0) PreviousResource = resourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = resourceInventory[i];

			if (CurrentResource->resourceName != PreviousResource->resourceName)
			{
				if (index == resourceStackIndex)
				{
					resourceImage = PreviousResource->resourceImage;
					return;
				}
				index++;
			}
			else if (CurrentResource->resourceName == PreviousResource->resourceName)
			{
				if (index == resourceStackIndex)
				{
					resourceImage = CurrentResource->resourceImage;
					return;
				}
			}
		}

		if (index == resourceStackIndex)
		{
			resourceImage = CurrentResource->resourceImage;
			return;
		}
	}*/
}

void UPlayerInventoryComponent::GetResourceCount(int32 resourceStackIndex, int32& resourceCount)
{
	int32 index = 0;
	for (TPair<FR, uint8>& resource : resourceMap)
	{
		if (index == resourceStackIndex)
		{
			resourceCount = resource.Value;
			return;
		}

		index++;
	}

	/*int32 ItemCount = 0;

	if (resourceInventory.Num() > 0)
	{
		AResource* CurrentResource = nullptr;
		AResource* PreviousResource = nullptr;

		for (int i = 0; i < resourceInventory.Num(); i++)
		{
			if (i == 0) PreviousResource = resourceInventory[0];
			else PreviousResource = CurrentResource;
			CurrentResource = resourceInventory[i];

			if (CurrentResource->resourceName != PreviousResource->resourceName)
			{
				if (index == resourceStackIndex)
				{
					resourceCount = ItemCount;
					return;
				}
				index++;
				ItemCount = 1;
			}
			else if (CurrentResource->resourceName == PreviousResource->resourceName)
			{
				ItemCount++;
			}
		}

		if (index == resourceStackIndex)
		{
			resourceCount = ItemCount;
			return;
		}

		ItemCount = 0;
	}*/
}

UTexture2D* UPlayerInventoryComponent::CheckCanCraft()
{
	UTexture2D* Image = nullptr;

	if (setIngredientsOneInv.Num() > 0)
	{
		if (setIngredientsTwoInv.Num() > 0)
		{
			FName Craftable = *setIngredientsOneInv[0]->CheckCanCombine(setIngredientsTwoInv[0]);
			if (Craftable != "")
			{
				AUsable* Usable = NewObject<AUsable>();
				if (Usable)
				{
					Usable->BuildUsable(Craftable);
					if (Usable->UsableImage)
					{
						Image = Usable->UsableImage;
						return Image;
					}
				}
			}
		}
	}

	return Image;
}

bool UPlayerInventoryComponent::AddUsable()
{
	if (setIngredientsOneInv.Num() > 0)
	{
		if (setIngredientsTwoInv.Num() > 0)
		{
			FName Craftable = *setIngredientsOneInv[0]->CheckCanCombine(setIngredientsTwoInv[0]);
			if (Craftable != "")
			{
				AUsable* Usable = NewObject<AUsable>();
				if (Usable)
				{
					Usable->BuildUsable(Craftable);
					AUsable* UsableToAdd = Cast<AUsable>(PotionMap[Usable->UsableID]->GetDefaultObject());
					FString Name = UsableToAdd->UsableName;
					usablesInventory.Add(UsableToAdd);

					setIngredientsOneInv.RemoveAt(0);
					setIngredientsTwoInv.RemoveAt(0);

					return true;
				}
			}
		}
	}

	return false;
}

bool UPlayerInventoryComponent::CheckCanCraftMore()
{
	if (setIngredientsOneInv.Num() > 0)
	{
		if (setIngredientsTwoInv.Num() > 0)
		{
			FName Craftable = *setIngredientsOneInv[0]->CheckCanCombine(setIngredientsTwoInv[0]);
			if (Craftable != "")
			{
				return true;
			}
		}
	}

	return false;
}

int32 UPlayerInventoryComponent::GetIngredientsCount(bool first, bool second)
{
	if (first)
	{
		return setIngredientsOneInv.Num();
	}

	if (second)
	{
		return setIngredientsTwoInv.Num();
	}

	return 0;
}

UTexture2D* UPlayerInventoryComponent::GetIngredientImage(bool first, bool second)
{
	if (first)
	{
		if (setIngredientsOneInv.Num() > 0)
		{
			return setIngredientsOneInv[0]->resourceImage;
		}
	}
	if (second)
	{
		if (setIngredientsTwoInv.Num() > 0)
		{
			return setIngredientsTwoInv[0]->resourceImage;
		}
	}

	return nullptr;
}

UTexture2D* UPlayerInventoryComponent::GetEquipmentImage(int32 index)
{
	UTexture2D* Image = nullptr;
	int ItemIndex = 1;

	if (weaponInventory.Num() > 0)
	{
		for (int i = 0; i < weaponInventory.Num(); i++)
		{
			if (i == index)
			{
				AWeapon* weapon = Cast<AWeapon>(weaponInventory[i]->GetDefaultObject());
				if (weapon)
				{
					if (weapon->ItemImage)
					{
						return weapon->ItemImage;
					}
				}
			}
		}
	}

	return Image;
}

UTexture2D* UPlayerInventoryComponent::GetGearImage(int32 index, int32& itemCount)
{
	UTexture2D* image = nullptr;
	int itemIndex = 0;
	itemCount = 0;

	if (index == -1) return nullptr;

	if (usablesInventory.Num() > 0)
	{
		AUsable* currentUsable = nullptr;
		AUsable* previousVisible = nullptr;

		for (int i = 0; i < usablesInventory.Num(); i++)
		{
			if (i == 0) previousVisible = usablesInventory[0];
			else previousVisible = currentUsable;
			currentUsable = usablesInventory[i];

			if (currentUsable->UsableName != previousVisible->UsableName)
			{
				if (itemIndex == index)
				{
					return image;
				}
				itemIndex++;
				itemCount = 1;
			}
			else if (currentUsable->UsableName == previousVisible->UsableName)
			{
				itemCount++;
			}

			if (itemIndex == index)
			{
				image = currentUsable->UsableImage;
			}
			else
			{
				itemCount = 0;
				image = nullptr;
			}
		}
	}

	return image;
}

void UPlayerInventoryComponent::SetGearUseIndex(int32 gearBoxIndex, int32 desiredInventoryIndex)
{
	int32 swapIndex = -1;
	int32 swapValue = -1;

	for (int i = 0; i < currentGearIndexes.Num(); i++)
	{
		if (i == gearBoxIndex)
		{
			swapValue = currentGearIndexes[i];
			currentGearIndexes[i] = desiredInventoryIndex;
		}

		if (i != gearBoxIndex && currentGearIndexes[i] == desiredInventoryIndex)
		{
			swapIndex = i;
		}
	}

	if (swapIndex >= 0)
	{
		currentGearIndexes[swapIndex] = swapValue;
	}
}

void UPlayerInventoryComponent::GetGearStack(int32 stackIndex, int32 slotIndex)
{
	int32 Index = 0;
	int32 StartIndex = 0;
	int32 EndIndex = 0;

	if (usablesInventory.Num() > 0)
	{
		AUsable* currentUsable = nullptr;
		AUsable* previousUsable = nullptr;

		for (int i = 0; i < usablesInventory.Num(); i++)
		{
			if (i == 0) previousUsable = usablesInventory[0];
			else previousUsable = currentUsable;
			currentUsable = usablesInventory[i];

			if (currentUsable->UsableName == previousUsable->UsableName)
			{
				if (Index == stackIndex)
				{
					if (i == 0) StartIndex = i;
				}
			}
			if (currentUsable->UsableName != previousUsable->UsableName)
			{
				Index++;
				if (Index == stackIndex)
				{
					StartIndex = i;
				}
			}
		}

		for (int i = StartIndex, j = 0; i < usablesInventory.Num(); j++)
		{
			if (j == 0) previousUsable = usablesInventory[i];
			else previousUsable = currentUsable;
			currentUsable = usablesInventory[i];

			if (currentUsable->UsableName == previousUsable->UsableName)
			{
				AddToGearSlot(usablesInventory[StartIndex], slotIndex);
				usablesInventory.RemoveAt(StartIndex);
			}
			else
			{
				return;
			}
		}
	}
}

void UPlayerInventoryComponent::RemoveGearStack(int32 slotIndex)
{
	switch (slotIndex)
	{
	case 0:
		while (gearSlotOneInventory.Num() > 0)
		{
			usablesInventory.Add(gearSlotOneInventory[0]);
			gearSlotOneInventory.RemoveAt(0);
		}
		break;
	case 1:
		while (gearSlotTwoInventory.Num() > 0)
		{
			usablesInventory.Add(gearSlotTwoInventory[0]);
			gearSlotTwoInventory.RemoveAt(0);
		}
		break;
	case 2:
		while (gearSlotThreeInventory.Num() > 0)
		{
			usablesInventory.Add(gearSlotThreeInventory[0]);
			gearSlotThreeInventory.RemoveAt(0);
		}
		break;
	case 3:
		while (gearSlotFourInventory.Num() > 0)
		{
			usablesInventory.Add(gearSlotFourInventory[0]);
			gearSlotFourInventory.RemoveAt(0);
		}
		break;
	}
}

void UPlayerInventoryComponent::SwapGearSlot(int32 firstSlot, int32 secondSlot)
{
	TArray<AUsable*> tempOne;
	TArray<AUsable*> tempTwo;

	switch (firstSlot)
	{
	case 0:
		tempOne = gearSlotOneInventory;
		break;
	case 1:
		tempOne = gearSlotTwoInventory;
		break;
	case 2:
		tempOne = gearSlotThreeInventory;
		break;
	case 3:
		tempOne = gearSlotFourInventory;
		break;
	}

	switch (secondSlot)
	{
	case 0:
		tempTwo = gearSlotOneInventory;
		gearSlotOneInventory = tempOne;
		break;
	case 1:
		tempTwo = gearSlotTwoInventory;
		gearSlotTwoInventory = tempOne;
		break;
	case 2:
		tempTwo = gearSlotThreeInventory;
		gearSlotThreeInventory = tempOne;
		break;
	case 3:
		tempTwo = gearSlotFourInventory;
		gearSlotFourInventory = tempOne;
		break;
	}

	switch (firstSlot)
	{
	case 0:
		gearSlotOneInventory = tempTwo;
		break;
	case 1:
		gearSlotTwoInventory = tempTwo;
		break;
	case 2:
		gearSlotThreeInventory = tempTwo;
		break;
	case 3:
		gearSlotFourInventory = tempTwo;
		break;
	}
}

void UPlayerInventoryComponent::GetGearSlotImageAndCount(int32 slotIndex, UTexture2D*& outImage, int32& count, bool& hasGear)
{
	switch (slotIndex)
	{
	case 0:
		if (gearSlotOneInventory.Num() > 0)
		{
			outImage = gearSlotOneInventory[0]->UsableImage;
			count = gearSlotOneInventory.Num();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	case 1:
		if (gearSlotTwoInventory.Num() > 0)
		{
			outImage = gearSlotTwoInventory[0]->UsableImage;
			count = gearSlotTwoInventory.Num();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	case 2:
		if (gearSlotThreeInventory.Num() > 0)
		{
			outImage = gearSlotThreeInventory[0]->UsableImage;
			count = gearSlotThreeInventory.Num();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	case 3:
		if (gearSlotFourInventory.Num() > 0)
		{
			outImage = gearSlotFourInventory[0]->UsableImage;
			count = gearSlotFourInventory.Num();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}
}

void UPlayerInventoryComponent::GetGearInventoryStackImageAndCount(int32 stackIndex, UTexture2D*& outImage, int32& count, bool& hasGear)
{
	int32 Index = 0;
	count = 0;
	hasGear = false;

	if (usablesInventory.Num() > 0)
	{
		AUsable* previousUsable = nullptr;
		AUsable* currentUsable = nullptr;

		for (int i = 0; i < usablesInventory.Num(); i++)
		{
			if (i == 0) previousUsable = usablesInventory[0];
			else previousUsable = currentUsable;
			currentUsable = usablesInventory[i];

			if (currentUsable->UsableName == previousUsable->UsableName)
			{
				count++;
			}
			else if (currentUsable->UsableName != previousUsable->UsableName)
			{
				if (Index == stackIndex)
				{
					outImage = previousUsable->UsableImage;
					hasGear = true;
					return;
				}
				Index++;
				count = 1;
			}
		}

		if (Index == stackIndex)
		{
			outImage = currentUsable->UsableImage;
			hasGear = true;
			return;
		}
	}
}

void UPlayerInventoryComponent::AddToGearSlot(AUsable* usableToAdd, int32 slotIndex)
{
	switch (slotIndex)
	{
	case 0:
		if (gearSlotOneInventory.Num() > 0)
		{
			if (gearSlotOneInventory[0]->UsableName != usableToAdd->UsableName)
			{
				while (gearSlotOneInventory.Num() > 0)
				{
					usablesInventory.Add(gearSlotOneInventory[0]);
					gearSlotOneInventory.RemoveAt(0);
				}
			}
		}
		gearSlotOneInventory.Add(usableToAdd);
		break;
	case 1:
		if (gearSlotTwoInventory.Num() > 0)
		{
			if (gearSlotTwoInventory[0]->UsableName != usableToAdd->UsableName)
			{
				while (gearSlotTwoInventory.Num() > 0)
				{
					usablesInventory.Add(gearSlotTwoInventory[0]);
					gearSlotTwoInventory.RemoveAt(0);
				}
			}
		}
		gearSlotTwoInventory.Add(usableToAdd);
		break;
	case 2:
		if (gearSlotThreeInventory.Num() > 0)
		{
			if (gearSlotThreeInventory[0]->UsableName != usableToAdd->UsableName)
			{
				while (gearSlotThreeInventory.Num() > 0)
				{
					usablesInventory.Add(gearSlotThreeInventory[0]);
					gearSlotThreeInventory.RemoveAt(0);
				}
			}
		}
		gearSlotThreeInventory.Add(usableToAdd);
		break;
	case 3:
		if (gearSlotFourInventory.Num() > 0)
		{
			if (gearSlotFourInventory[0]->UsableName != usableToAdd->UsableName)
			{
				while (gearSlotFourInventory.Num() > 0)
				{
					usablesInventory.Add(gearSlotFourInventory[0]);
					gearSlotFourInventory.RemoveAt(0);
				}
			}
		}
		gearSlotFourInventory.Add(usableToAdd);
		break;
	}
}

//void UPlayerInventoryComponent::QuickSortUsables(TArray<AUsable*> inventory, int32 low, int32 high)
//{
//	if (low < high)
//	{
//		int32 pi = Partition(inventory, low, high);
//
//		QuickSortUsables(inventory, low, pi - 1);
//		QuickSortUsables(inventory, pi + 1, high);
//	}
//}
//
//int32 UPlayerInventoryComponent::Partition(TArray<AUsable*> inventory, int32 low, int32 high)
//{
//	AUsable* pivot = inventory[high];
//
//	int32 i = low - 1;
//
//	for (int32 j = low; j <= high - 1; j++)
//	{
//		if (inventory[j]->UsableID < pivot->UsableID)
//		{
//			i++;
//			Swap(i, j);
//		}
//	}
//
//	Swap(i + 1, high);
//	return i + 1;
//}
//
//void UPlayerInventoryComponent::Swap(int32 i, int32 j)
//{
//	AUsable* temp = usablesInventory[i];
//	usablesInventory[i] = usablesInventory[j];
//	usablesInventory[j] = temp;
//}
//
//void UPlayerInventoryComponent::DutchQuickSort(TArray<AUsable*> inventory, int left, int right)
//{
//	if (right <= left) return;
//
//	int i = 0, j = 0;
//
//	DutchPartition(inventory, left, right, i, j);
//
//	DutchQuickSort(inventory, left, j);
//	DutchQuickSort(inventory, i, right);
//}
//
//void UPlayerInventoryComponent::DutchPartition(TArray<AUsable*> Arr, int left, int right, int i, int j)
//{
//	i = left - 1;
//	j = right;
//	int p = left - 1, q = right;
//	int v = Arr[right]->UsableID;
//
//	while (true)
//	{
//		while (Arr[i++]->UsableID < v);
//
//		while (v < Arr[j--]->UsableID)
//		{
//			if (j == left) break;
//		}
//
//		if (i >= j) break;
//
//		Swap(i, j);
//
//		if (Arr[i]->UsableID == v)
//		{
//			p++;
//			Swap(p, i);
//		}
//
//		if (Arr[j]->UsableID == v)
//		{
//			q--;
//			Swap(j, q);
//		}
//	}
//
//	Swap(i, right);
//
//	j = i - 1;
//	for (int k = left; k < p; k++, j--)
//	{
//		Swap(k, j);
//	}
//
//	i = i + 1;
//	for (int k = right - 1; k > q; k--, i++)
//	{
//		Swap(i, k);
//	}
//}
