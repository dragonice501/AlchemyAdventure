// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource.h"

void AResource::BuildResource(const FResourcePropertyTable* row)
{
	if (row)
	{
		resourceName = row->resourceName;
		combinableResources = row->combinableResources;
		combineResults = row->combineResults;
		resourceImage = row->resourceImage;
		resourceElement = row->resourceElement;
	}
}

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
			resourceName = ResourceRow->resourceName;
			combinableResources = ResourceRow->combinableResources;
			combineResults = ResourceRow->combineResults;
			resourceImage = ResourceRow->resourceImage;
			resourceElement = ResourceRow->resourceElement;
		}
	}
}

FString AResource::CheckCanCombine(AResource* OtherResource)
{
	FString Result;
	if (OtherResource)
	{
		for (int i = 0; i < combinableResources.Num(); i++)
		{
			if (combinableResources[i] == OtherResource->resourceName)
			{
				Result = combineResults[i];
				return Result;
			}
		}
	}

	return Result;
}