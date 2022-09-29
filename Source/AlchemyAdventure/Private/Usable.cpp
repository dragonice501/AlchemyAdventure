// Fill out your copyright notice in the Description page of Project Settings.


#include "Usable.h"

void AUsable::BuildUsable(FName Name)
{
	FString UsableTablePath = TEXT("DataTable'/Game/Data/DT_Usable.DT_Usable'");
	UDataTable* UsableDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *UsableTablePath));
	if (UsableDataTable)
	{
		FUsablePropertyTable* UsableRow = nullptr;
		UsableRow = UsableDataTable->FindRow<FUsablePropertyTable>(Name, TEXT(""));
		if (UsableRow)
		{
			UsableName = UsableRow->UsableName;
			UsableImage = UsableRow->UsableImage;
		}
	}
}