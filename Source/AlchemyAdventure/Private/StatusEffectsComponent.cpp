// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffectsComponent.h"
#include "StatusEffect.h"
#include "AttackBoostStatusEffect.h"
#include "HealStatusEffect.h"
#include "Weapon.h"

// Sets default values for this component's properties
UStatusEffectsComponent::UStatusEffectsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatusEffectsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UStatusEffectsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*TArray<uint8> removeIndeces;
	uint8 index = 0;
	for (UStatusEffect* statusEffect : mStatusEffects)
	{
		statusEffect->effectDurationTime -= DeltaTime;
		if (statusEffect->effectDurationTime <= 0)
		{
			removeIndeces.Add(index);
		}
	}

	for (size_t i = 0; i < removeIndeces.Num(); i++)
	{
		mStatusEffects.RemoveAt(removeIndeces[i]);
	}*/
}

void UStatusEffectsComponent::AddStatusEffect(FDataTableRowHandle statusEffectDataTable, ACharacter* effectedCharacter)
{
	if(effectedCharacter)
	{
		UDataTable* dataTable = Cast<UDataTable>(statusEffectDataTable.DataTable);
		if(dataTable)
		{
			FStatusEffectPropertyTable* row = dataTable->FindRow<FStatusEffectPropertyTable>(statusEffectDataTable.RowName, TEXT(""));
			if (row)
			{
				UStatusEffect* newStatusEffect = NewObject<UStatusEffect>();

				switch (row->effectType)
				{
				case EEffectType::EED_ATTACK:
				{
					newStatusEffect = NewObject<UAttackBoostStatusEffect>();
					newStatusEffect->BuildEffect(row);
					break;
				}
				case EEffectType::EED_DEFENSE:
				{
					break;
				}
				case EEffectType::EED_HEALTH:
				{
					newStatusEffect = NewObject<UHealStatusEffect>();
					newStatusEffect->BuildEffect(row);
					break;
				}
				case EEffectType::EED_MOBILITY:
				{
					newStatusEffect = NewObject<UHealStatusEffect>();
					newStatusEffect->BuildEffect(row);
					break;
				}
				}

				if(newStatusEffect)
				{
					newStatusEffect->effectedCharacter = effectedCharacter;
					newStatusEffect->BuildEffect(row);

					switch (newStatusEffect->effectDurationType)
					{
					case EEffectDuration::EED_INSTANT:
					{
						newStatusEffect->ApplyEffect();
						break;
					}
					case EEffectDuration::EED_TEMPORARY:
					{
						mStatusEffects.Add(newStatusEffect);
						break;
					}
					case EEffectDuration::EED_TIMED:
					{
						mStatusEffects.Add(newStatusEffect);
						break;
					}
					}
				}
			}
		}
	}
}

void UStatusEffectsComponent::RemoveStatusEffect(int32 statusEffectIndex)
{
	if (statusEffectIndex <= mStatusEffects.Num() - 1)
	{
		mStatusEffects.RemoveAt(statusEffectIndex);
	}
}

void UStatusEffectsComponent::ModifyAttackDamage(int32& damage)
{
	for (UStatusEffect* statusEffect : mStatusEffects)
	{
		if (statusEffect->effectType == EEffectType::EED_ATTACK)
		{
			UAttackBoostStatusEffect* attackBoost = Cast<UAttackBoostStatusEffect>(statusEffect);
			if(attackBoost)
			{
				damage = statusEffect->ModifyInt(damage);
			}
		}
	}
}
