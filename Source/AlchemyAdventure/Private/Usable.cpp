// Fill out your copyright notice in the Description page of Project Settings.


#include "Usable.h"

void AUsable::BuildUsable(FName Name)
{
	FString UsableTablePath = TEXT("DataTable'/Game/Data/DT_Usables.DT_Usables'");
	UDataTable* UsableDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *UsableTablePath));
	if (UsableDataTable)
	{
		FUsablePropertyTable* UsableRow = nullptr;
		UsableRow = UsableDataTable->FindRow<FUsablePropertyTable>(Name, TEXT(""));
		if (UsableRow)
		{
			UsableID = UsableRow->UsableID;
			UsableName = UsableRow->UsableName;
			UsableImage = UsableRow->UsableImage;
			StatusEffect = UsableRow->StatusEffect;
			StatusEffectTime = UsableRow->StatusEffectTime;

			switch (StatusEffect)
			{
			case EStatusEffect::ESE_None:
				break;
			case EStatusEffect::ESE_Attack:
				AttackModifier = UsableRow->AttackModifier;
				break;
			case EStatusEffect::ESE_Heal:
				HealAmount = UsableRow->HealAmount;
				break;
			case EStatusEffect::ESE_Defense:
				DefenseModifier = UsableRow->DefenseModifier;
				break;
			case EStatusEffect::ESE_Mobility:
				MobilityModifier = UsableRow->MobilityModifier;
				break;
			default:
				break;
			}
		}
	}
}