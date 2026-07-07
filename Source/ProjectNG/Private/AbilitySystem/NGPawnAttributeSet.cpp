// Copyright (c) 2025 TeamNG. All Rights Reserved.
#include "AbilitySystem/NGPawnAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Pawn/NGPawnBase.h"

void UNGPawnAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, AbilityPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, CriticalRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, DodgeRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, PhysicalDefense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, MagicDefense, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, Income, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNGPawnAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, Health)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, MaxHealth)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, Mana)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, MaxMana)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, ManaRegeneration)

DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, AttackDamage)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, AbilityPower)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, AttackRange)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, AttackSpeed)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, CriticalRate)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, DodgeRate)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, PhysicalDefense)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, MagicDefense)

DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, Agility)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, Income)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, MoveSpeed)
DEFAULT_REPLICATION_IMPLEMENTATION(UNGPawnAttributeSet, Star)

void UNGPawnAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute() && GetHealth() <= 0.f)
	{
		if (ANGPawnBase* TargetCharacter = Cast<ANGPawnBase>(Data.Target.GetAvatarActor()))
		{
			TargetCharacter->Die();
		}
	}
}
