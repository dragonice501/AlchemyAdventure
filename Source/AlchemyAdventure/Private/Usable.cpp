// Fill out your copyright notice in the Description page of Project Settings.


#include "Usable.h"

void AUsable::BuildUsable(FName Name)
{
	FString UsableTablePath = TEXT("DataTable'/Game/Data/DT_Usables.DT_Usables'");
	UDataTable* UsableDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *UsableTablePath));
	if (UsableDataTable)
	{
		FUsablePropertyTable* usableRow = nullptr;
		usableRow = UsableDataTable->FindRow<FUsablePropertyTable>(Name, TEXT(""));
		if (usableRow)
		{
			usableName = usableRow->usableName;
			usableImage = usableRow->usableImage;
		}
	}
}

void AUsable::BuildUsable(const FUsablePropertyTable* row)
{
	if (row)
	{
		usableName = row->usableName;
		usableImage = row->usableImage;
	}
}
