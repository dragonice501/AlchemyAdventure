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
			statusEffect = usableRow->statusEffect;
			statusEffectTime = usableRow->statusEffectTime;

			switch (statusEffect)
			{
			case EStatusEffect::ESE_None:
				break;
			case EStatusEffect::ESE_Attack:
				attackModifier = usableRow->attackModifier;
				break;
			case EStatusEffect::ESE_Heal:
				healAmount = usableRow->healAmount;
				break;
			case EStatusEffect::ESE_Defense:
				defenseModifier = usableRow->defenseModifier;
				break;
			case EStatusEffect::ESE_Mobility:
				mobilityModifier = usableRow->mobilityModifier;
				break;
			default:
				break;
			}
		}
	}
}

void AUsable::BuildUsable(const FUsablePropertyTable* row)
{
	if (row)
	{
		usableName = row->usableName;
		usableImage = row->usableImage;
		statusEffect = row->statusEffect;
		statusEffectTime = row->statusEffectTime;

		switch (statusEffect)
		{
		case EStatusEffect::ESE_None:
			break;
		case EStatusEffect::ESE_Attack:
			attackModifier = row->attackModifier;
			break;
		case EStatusEffect::ESE_Heal:
			healAmount = row->healAmount;
			break;
		case EStatusEffect::ESE_Defense:
			defenseModifier = row->defenseModifier;
			break;
		case EStatusEffect::ESE_Mobility:
			mobilityModifier = row->mobilityModifier;
			break;
		}
	}
}
