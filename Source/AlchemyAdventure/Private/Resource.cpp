// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource.h"

void AResource::BuildResource(FName ItemName)
{
	FString ResourceTablePath = TEXT("DataTable'/Game/Data/DT_Resources.DT_Resources'");
	UDataTable* ResourceDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ResourceTablePath));
	if (ResourceDataTable)
	{
		FResourcePropertyTable* ResourceRow = nullptr;
		ResourceRow = ResourceDataTable->FindRow<FResourcePropertyTable>(ItemName, TEXT(""));
		if (ResourceRow)
		{
			ResourceID = ResourceRow->ResourceID;
			ResourceName = ResourceRow->ResourceName;
			CombinableResources = ResourceRow->CombinableResources;
			CombineResults = ResourceRow->CombineResults;
			resourceImage = ResourceRow->resourceImage;
			ResourceElement = ResourceRow->ResourceElement;
		}
	}
}

FString AResource::CheckCanCombine(AResource* OtherResource)
{
	FString Result;
	if (OtherResource)
	{
		for (int i = 0; i < CombinableResources.Num(); i++)
		{
			if (CombinableResources[i] == OtherResource->ResourceName)
			{
				Result = CombineResults[i];
				return Result;
			}
		}
	}

	return Result;
}