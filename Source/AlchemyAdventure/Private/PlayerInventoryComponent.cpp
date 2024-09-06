// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"
#include "MainCharacter.h"
#include "Resource.h"
#include "StatusEffect.h"
#include "Usable.h"
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

void UPlayerInventoryComponent::AddToResourceInventory(const FDataTableRowHandle& itemDataTable)
{
	FResourcePropertyTable* row = itemDataTable.GetRow<FResourcePropertyTable>(itemDataTable.RowName.ToString());

	for (TPair<FR, uint8>& resource : resourceMap)
	{
		if (row->resourceName == resource.Key.resourceName)
		{
			resource.Value++;
			return;
		}
	}

	if(row)
	{
		FR newResource;
		newResource.BuildResource(row);
		resourceMap.Add(newResource, 1);
	}
}

void UPlayerInventoryComponent::AddToResourceInventory(const FR& resource, uint8 count)
{
	for (TPair<FR, uint8>& r : resourceMap)
	{
		if (r.Key.resourceName == resource.resourceName)
		{
			r.Value += count;
			return;
		}
	}

	resourceMap.Add(resource, count);
}

void UPlayerInventoryComponent::AddToUsablesInventory(const FU& usable, const uint8 count)
{
	int32 index = 0;
	for (TPair<FU, uint8>& u : usableMap)
	{
		if (u.Key == usable)
		{
			u.Value += count;
			return;
		}
	}

	usableMap.Add(usable, count);
}

void UPlayerInventoryComponent::AddToWeaponInventory(const FDataTableRowHandle& weaponDataTable)
{
	UDataTable* dataTable = Cast<UDataTable>(weaponDataTable.DataTable);
	FWeaponPropertyTable* row = dataTable->FindRow<FWeaponPropertyTable>(FName(weaponDataTable.RowName), TEXT(""));
	if(row)
	{
		FW newWeapon;
		newWeapon.BuildWeapon(row, weaponDataTable.RowName);
		weaponMap.Add(newWeapon);
		//weaponMap.Sort();
	}
}

void UPlayerInventoryComponent::RemoveResourceFromInventory(const FR& resource, uint8 count)
{
	for (TPair<FR, uint8>& r : resourceMap)
	{
		if (r.Key.resourceName == resource.resourceName)
		{
			r.Value -= count;
			if (r.Value <= 0)
			{
				resourceMap.Remove(r.Key);
			}
			return;
		}
	}
}

bool UPlayerInventoryComponent::RemoveAndSetIngredient(int32 resourceStackIndex, int32 resourceSelectIndex, bool firstIngredient, UTexture2D*& resourceImage)
{
	int32 resourceIndex = 0;

	int32 index = 0;
	for (TPair<FR, uint8>& resource : resourceMap)
	{
		// Desired ingredient found
		if (index == resourceStackIndex)
		{
			resourceImage = resource.Key.resourceImage;

			// first ingredient selected
			if (firstIngredient)
			{
				// no ingredient placed yet
				if (firstIngredientMap.IsEmpty())
				{
					firstIngredientMap.Add(resource.Key, 1);
					RemoveResourceFromInventory(resource.Key, 1);
				}
				// ingredient already in place
				else if (firstIngredientMap.begin())
				{
					// if same ingredient increment num
					if (firstIngredientMap.begin()->Key == resource.Key)
					{
						firstIngredientMap.begin()->Value++;
						RemoveResourceFromInventory(resource.Key, 1);
					}
					// if different ingredient return to inventory
					else
					{
						AddToResourceInventory(firstIngredientMap.begin().Key(), firstIngredientMap.begin().Value());

						firstIngredientMap.begin().Key() = resource.Key;
						firstIngredientMap.begin().Value() = 1;

						RemoveResourceFromInventory(resource.Key, 1);
					}
				}
			}
			else
			{
				// no ingredient placed yet
				if (secondIngredientMap.IsEmpty())
				{
					secondIngredientMap.Add(resource.Key, 1);
					RemoveResourceFromInventory(resource.Key, 1);
				}
				// ingredient already in place
				else if (secondIngredientMap.begin())
				{
					// if same ingredient increment num
					if (secondIngredientMap.begin()->Key == resource.Key)
					{
						secondIngredientMap.begin()->Value++;
						RemoveResourceFromInventory(resource.Key, 1);
					}
					// if different ingredient return to inventory
					else
					{
						AddToResourceInventory(secondIngredientMap.begin().Key(), secondIngredientMap.begin().Value());

						secondIngredientMap.begin().Key() = resource.Key;
						secondIngredientMap.begin().Value() = 1;

						RemoveResourceFromInventory(resource.Key, 1);
					}
				}
			}

			return true;
		}

		index++;
	}

	/*if (resourceInventory.Num() > 0)
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
	}*/

	return false;
}

void UPlayerInventoryComponent::ResetCraftingIngredients(bool resetFirst, bool resetSecond)
{
	if (firstIngredientMap.begin() && resetFirst)
	{
		AddToResourceInventory(firstIngredientMap.begin().Key(), firstIngredientMap.begin().Value());

		firstIngredientMap.Empty();
	}

	if (secondIngredientMap.begin() && resetSecond)
	{
		AddToResourceInventory(secondIngredientMap.begin().Key(), secondIngredientMap.begin().Value());

		secondIngredientMap.Empty();
	}
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
	UTexture2D* image = nullptr;

	uint8 index = 0;
	if (firstIngredientMap.begin() && secondIngredientMap.begin())
	{
		for (FString combinable : firstIngredientMap.begin().Key().combinableResources)
		{
			if (combinable == secondIngredientMap.begin().Key().resourceName)
			{
				if(!usableDataTable.IsNull())
				{
					UDataTable* dataTable = Cast<UDataTable>(usableDataTable.DataTable);
					if(dataTable)
					{
						FUsablePropertyTable* row = dataTable->FindRow<FUsablePropertyTable>(FName(firstIngredientMap.begin().Key().combineResults[index]), TEXT(""));
						if (row)
						{
							image = row->usableImage;
						}
					}
				}
			}

			index++;
		}
	}

	/*if (setIngredientsOneInv.Num() > 0)
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
					if (Usable->usableImage)
					{
						Image = Usable->usableImage;
						return Image;
					}
				}
			}
		}
	}*/

	return image;
}

bool UPlayerInventoryComponent::AddUsable()
{
	uint8 index = 0;
	if (firstIngredientMap.begin() && secondIngredientMap.begin())
	{
		for (FString combinable : firstIngredientMap.begin().Key().combinableResources)
		{
			if (combinable == secondIngredientMap.begin().Key().resourceName)
			{
				if (!usableDataTable.IsNull())
				{
					UDataTable* dataTable = Cast<UDataTable>(usableDataTable.DataTable);
					if (dataTable)
					{
						FUsablePropertyTable* row = dataTable->FindRow<FUsablePropertyTable>(FName(firstIngredientMap.begin().Key().combineResults[index]), TEXT(""));
						if (row)
						{
							if (--firstIngredientMap.begin().Value() == 0) firstIngredientMap.Empty();
							if (--secondIngredientMap.begin().Value() == 0) secondIngredientMap.Empty();

							for (TPair<FU, uint8>& u : usableMap)
							{
								if (u.Key.usableName == row->usableName)
								{
									u.Value += 1;
									return true;
								}
							}

							FU newUsable;
							newUsable.BuildUsable(row);
							usableMap.Add(newUsable, 1);

							return true;
						}
					}
				}
			}

			index++;
		}
	}

	/*if (setIngredientsOneInv.Num() > 0)
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
					//AUsable* UsableToAdd = Cast<AUsable>(PotionMap[Usable->usableID]->GetDefaultObject());
					//FString Name = UsableToAdd->usableName;
					//usablesInventory.Add(UsableToAdd);

					setIngredientsOneInv.RemoveAt(0);
					setIngredientsTwoInv.RemoveAt(0);

					return true;
				}
			}
		}
	}*/

	return false;
}

bool UPlayerInventoryComponent::CheckCanCraftMore()
{
	if (firstIngredientMap.begin() && secondIngredientMap.begin()) return true;

	/*if (setIngredientsOneInv.Num() > 0)
	{
		if (setIngredientsTwoInv.Num() > 0)
		{
			FName Craftable = *setIngredientsOneInv[0]->CheckCanCombine(setIngredientsTwoInv[0]);
			if (Craftable != "")
			{
				return true;
			}
		}
	}*/

	return false;
}

UTexture2D* UPlayerInventoryComponent::GetIngredientImage(bool first, bool second)
{
	if (first && firstIngredientMap.begin())
	{
		return firstIngredientMap.begin().Key().resourceImage;
	}

	if (second && secondIngredientMap.begin())
	{
		return secondIngredientMap.begin().Key().resourceImage;
	}

	return nullptr;
}

int32 UPlayerInventoryComponent::GetIngredientsCount(bool first, bool second)
{
	if (first && firstIngredientMap.begin())
	{
		return firstIngredientMap.begin().Value();
	}

	if (second && secondIngredientMap.begin())
	{
		return secondIngredientMap.begin().Value();
	}

	return 0;
}

UTexture2D* UPlayerInventoryComponent::GetInventoryWeaponImage(int32 index)
{
	UTexture2D* image = nullptr;
	/*if (weaponInventory.Num() > 0)
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
	}*/

	if (weaponMap.Num() > 0 && index <= weaponMap.Num() - 1)
	{
		image = weaponMap[index].weaponImage;
	}

	return image;
}

void UPlayerInventoryComponent::SetGearUseIndex(int32 gearBoxIndex, int32 desiredInventoryIndex)
{
	/*int32 swapIndex = -1;
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
	}*/
}

void UPlayerInventoryComponent::SetGearSlot(int32 stackIndex, int32 slotIndex)
{
	/*int32 Index = 0;
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

			if (currentUsable->usableName == previousUsable->usableName)
			{
				if (Index == stackIndex)
				{
					if (i == 0) StartIndex = i;
				}
			}
			if (currentUsable->usableName != previousUsable->usableName)
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

			if (currentUsable->usableName == previousUsable->usableName)
			{
				AddToGearSlot(usablesInventory[StartIndex], slotIndex);
				usablesInventory.RemoveAt(StartIndex);
			}
			else
			{
				return;
			}
		}
	}*/

	int32 index = 0;
	for (TPair<FU, uint8>& usable : usableMap)
	{
		if (index == stackIndex)
		{
			switch (slotIndex)
			{
			case 0:
			{
				// Gear slot already occupied
				if (gearSlotOneMap.begin())
				{
					// Gear is the same
					if (gearSlotOneMap.begin().Key() == usable.Key)
					{
						gearSlotOneMap.begin().Value() += usable.Value;
						usableMap.Remove(usable.Key);
					}
					// Gear is different
					else
					{
						// Add gear back to inventory
						AddToUsablesInventory(gearSlotOneMap.begin().Key(), gearSlotOneMap.begin().Value());

						// Add new gear to slot
						gearSlotOneMap.Empty();
						gearSlotOneMap.Add(usable);
						usableMap.Remove(usable.Key);
					}
				}
				// Move gear from inventory to gear slot
				else
				{
					gearSlotOneMap.Add(usable);
					usableMap.Remove(usable.Key);
				}
				break;
			}
			case 1:
			{
				// Gear slot already occupied
				if (gearSlotTwoMap.begin())
				{
					// Gear is the same
					if (gearSlotTwoMap.begin().Key() == usable.Key)
					{
						gearSlotTwoMap.begin().Value() += usable.Value;
						usableMap.Remove(usable.Key);
					}
					// Gear is different
					else
					{
						// Add gear back to inventory
						AddToUsablesInventory(gearSlotTwoMap.begin().Key(), gearSlotTwoMap.begin().Value());

						// Add new gear to slot
						gearSlotTwoMap.Empty();
						gearSlotTwoMap.Add(usable);
						usableMap.Remove(usable.Key);
					}
				}
				// Move gear from inventory to gear slot
				else
				{
					gearSlotTwoMap.Add(usable);
					usableMap.Remove(usable.Key);
				}
				break;
			}
			case 2:
			{
				// Gear slot already occupied
				if (gearSlotThreeMap.begin())
				{
					// Gear is the same
					if (gearSlotThreeMap.begin().Key() == usable.Key)
					{
						gearSlotThreeMap.begin().Value() += usable.Value;
						usableMap.Remove(usable.Key);
					}
					// Gear is different
					else
					{
						// Add gear back to inventory
						AddToUsablesInventory(gearSlotThreeMap.begin().Key(), gearSlotThreeMap.begin().Value());

						// Add new gear to slot
						gearSlotThreeMap.Empty();
						gearSlotThreeMap.Add(usable);
						usableMap.Remove(usable.Key);
					}
				}
				// Move gear from inventory to gear slot
				else
				{
					gearSlotThreeMap.Add(usable);
					usableMap.Remove(usable.Key);
				}
				break;
			}
			case 3:
			{
				// Gear slot already occupied
				if (gearSlotFourMap.begin())
				{
					// Gear is the same
					if (gearSlotFourMap.begin().Key() == usable.Key)
					{
						gearSlotFourMap.begin().Value() += usable.Value;
						usableMap.Remove(usable.Key);
					}
					// Gear is different
					else
					{
						// Add gear back to inventory
						AddToUsablesInventory(gearSlotFourMap.begin().Key(), gearSlotFourMap.begin().Value());

						// Add new gear to slot
						gearSlotFourMap.Empty();
						gearSlotFourMap.Add(usable);
						usableMap.Remove(usable.Key);
					}
				}
				// Move gear from inventory to gear slot
				else
				{
					gearSlotFourMap.Add(usable);
					usableMap.Remove(usable.Key);
				}
				break;
			}
			}

			return;
		}

		index++;
	}
}

void UPlayerInventoryComponent::EmptyGearAtSlot(int32 slotIndex)
{
	/*switch (slotIndex)
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
	}*/

	switch (slotIndex)
	{
	case 0:
	{
		if (gearSlotOneMap.begin())
		{
			AddToUsablesInventory(gearSlotOneMap.begin().Key(), gearSlotOneMap.begin().Value());
			gearSlotOneMap.Empty();
		}
		break;
	}
	case 1:
	{
		if (gearSlotTwoMap.begin())
		{
			AddToUsablesInventory(gearSlotTwoMap.begin().Key(), gearSlotTwoMap.begin().Value());
			gearSlotTwoMap.Empty();
		}
		break;
	}
	case 2:
	{
		if (gearSlotThreeMap.begin())
		{
			AddToUsablesInventory(gearSlotThreeMap.begin().Key(), gearSlotThreeMap.begin().Value());
			gearSlotThreeMap.Empty();
		}
		break;
	}
	case 3:
	{
		if (gearSlotFourMap.begin())
		{
			AddToUsablesInventory(gearSlotFourMap.begin().Key(), gearSlotFourMap.begin().Value());
			gearSlotFourMap.Empty();
		}
		break;
	}
	}
}

void UPlayerInventoryComponent::RemoveFromGearSlot(int32 gearSlotIndex, int32 count)
{
	switch (gearSlotIndex)
	{
	case 0:
	{
		if (gearSlotOneMap.begin().Value() > 0)
		{
			gearSlotOneMap.begin().Value() -= count;
			if (gearSlotOneMap.begin().Value() <= 0)
			{
				gearSlotOneMap.Empty();
			}
		}
		break;
	}
	case 1:
	{
		if (gearSlotTwoMap.begin().Value() > 0)
		{
			gearSlotTwoMap.begin().Value() -= count;
			if (gearSlotTwoMap.begin().Value() <= 0)
			{
				gearSlotTwoMap.Empty();
			}
		}
		break;
	}
	case 2:
	{
		if (gearSlotThreeMap.begin().Value() > 0)
		{
			gearSlotThreeMap.begin().Value() -= count;
			if (gearSlotThreeMap.begin().Value() <= 0)
			{
				gearSlotThreeMap.Empty();
			}
		}
		break;
	}
	case 3:
	{
		if (gearSlotFourMap.begin().Value() > 0)
		{
			gearSlotFourMap.begin().Value() -= count;
			if (gearSlotFourMap.begin().Value() <= 0)
			{
				gearSlotFourMap.Empty();
			}
		}
		break;
	}
	}
}

void UPlayerInventoryComponent::UseGearInSlot(int32 gearSlotIndex)
{
	if(AMainCharacter* character = Cast<AMainCharacter>(GetOwner()))
	{
		switch (gearSlotIndex)
		{
		case 0:
		{
			if (gearSlotOneMap.begin() && gearSlotOneMap.begin().Key().statusEffect)
			{
				gearSlotOneMap.begin().Key().statusEffect.GetDefaultObject()->ApplyEffect(character);
			}
			break;
		}
		case 1:
		{
			if (gearSlotTwoMap.begin())
			{
				if (gearSlotTwoMap.begin() && gearSlotTwoMap.begin().Key().statusEffect)
				{
					gearSlotTwoMap.begin().Key().statusEffect.GetDefaultObject()->ApplyEffect(character);
				}
			}
			break;
		}
		case 2:
		{
			if (gearSlotThreeMap.begin())
			{
				if (gearSlotThreeMap.begin() && gearSlotThreeMap.begin().Key().statusEffect)
				{
					gearSlotThreeMap.begin().Key().statusEffect.GetDefaultObject()->ApplyEffect(character);
				}
			}
			break;
		}
		case 3:
		{
			if (gearSlotFourMap.begin())
			{
				if (gearSlotFourMap.begin() && gearSlotFourMap.begin().Key().statusEffect)
				{
					gearSlotFourMap.begin().Key().statusEffect.GetDefaultObject()->ApplyEffect(character);
				}
			}
			break;
		}
		}
	}
}

void UPlayerInventoryComponent::GetGearSlotImageAndCount(int32 slotIndex, UTexture2D*& outImage, int32& count, bool& hasGear)
{
	/*switch (slotIndex)
	{
	case 0:
		if (gearSlotOneInventory.Num() > 0)
		{
			outImage = gearSlotOneInventory[0]->usableImage;
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
			outImage = gearSlotTwoInventory[0]->usableImage;
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
			outImage = gearSlotThreeInventory[0]->usableImage;
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
			outImage = gearSlotFourInventory[0]->usableImage;
			count = gearSlotFourInventory.Num();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}*/

	switch (slotIndex)
	{
	case 0:
	{
		if (gearSlotOneMap.begin())
		{
			outImage = gearSlotOneMap.begin().Key().usableImage;
			count = gearSlotOneMap.begin().Value();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}
	case 1:
	{
		if (gearSlotTwoMap.begin())
		{
			outImage = gearSlotTwoMap.begin().Key().usableImage;
			count = gearSlotTwoMap.begin().Value();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}
	case 2:
	{
		if (gearSlotThreeMap.begin())
		{
			outImage = gearSlotThreeMap.begin().Key().usableImage;
			count = gearSlotThreeMap.begin().Value();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}
	case 3:
	{
		if (gearSlotFourMap.begin())
		{
			outImage = gearSlotFourMap.begin().Key().usableImage;
			count = gearSlotFourMap.begin().Value();
			hasGear = true;
		}
		else
		{
			hasGear = false;
		}
		break;
	}
	}
}

void UPlayerInventoryComponent::GetGearInventoryStackImageAndCount(int32 stackIndex, UTexture2D*& outImage, int32& count, bool& hasGear)
{
	/*int32 Index = 0;
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

			if (currentUsable->usableName == previousUsable->usableName)
			{
				count++;
			}
			else if (currentUsable->usableName != previousUsable->usableName)
			{
				if (Index == stackIndex)
				{
					outImage = previousUsable->usableImage;
					hasGear = true;
					return;
				}
				Index++;
				count = 1;
			}
		}

		if (Index == stackIndex)
		{
			outImage = currentUsable->usableImage;
			hasGear = true;
			return;
		}
	}*/

	int32 index = 0;
	for (TPair<FU, uint8>& usable : usableMap)
	{
		if (index == stackIndex)
		{
			outImage = usable.Key.usableImage;
			count = usable.Value;
			hasGear = true;
			return;
		}

		index++;
	}

	hasGear = false;
}