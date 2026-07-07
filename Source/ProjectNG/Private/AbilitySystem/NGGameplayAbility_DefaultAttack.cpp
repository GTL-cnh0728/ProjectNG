// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/NGGameplayAbility_DefaultAttack.h"

#include "AbilitySystem/NGPawnAttributeSet.h"

void UNGGameplayAbility_DefaultAttack::RegerateMana(const ANGPawnBase* Unit) const
{
	if (UNGAbilitySystemComponent* AbilitySystemComponent = Unit->GetNGAbilitySystemComponent())
	{
		if (const UNGPawnAttributeSet* AttributeSet = AbilitySystemComponent->GetSet<UNGPawnAttributeSet>())
		{
			float ManaRegenValue = AttributeSet->GetManaRegeneration();

			if (ManaRegenValue > 0.f)
			{
				UE_LOG(LogTemp, Log, TEXT("%s: RegenerateMana - %f"), *Unit->GetName(), ManaRegenValue);
				
				AbilitySystemComponent->ApplyModToAttribute(
				UNGPawnAttributeSet::GetManaAttribute(),
				EGameplayModOp::Additive,
				ManaRegenValue);
			}
		}
	}
}

void UNGGameplayAbility_DefaultAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UNGGameplayAbility_DefaultAttack::OnAttackReceived(FGameplayEventData Payload)
{
	Super::OnAttackReceived(Payload);
	
	RegerateMana(GetNGPawnFromActorInfo());
}
