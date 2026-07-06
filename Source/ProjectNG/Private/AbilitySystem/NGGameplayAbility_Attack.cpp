// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/NGGameplayAbility_Attack.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/NGPawnAttributeSet.h"
#include "Core/NGGameplayTags.h"

void UNGGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ANGPawnBase* Unit = GetNGPawnFromActorInfo();
	if (!Unit)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNGGameplayAbility_Attack::ActivateAbility - No Unit"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	TossTargetData = *TriggerEventData;
}

void UNGGameplayAbility_Attack::OnAttackReceived(FGameplayEventData Payload)
{
	ANGPawnBase* OwningActor = GetNGPawnFromActorInfo();
	UNGAbilitySystemComponent* OwnerASC = OwningActor ? OwningActor->GetNGAbilitySystemComponent() : nullptr;
	if (!OwnerASC)	return;
	
	SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	
	//Effect에다가 태그를 한개 달아 보내기 위한 과정
	FGameplayTag AbilityTag;
	for (const FGameplayTag& Tag : GetAssetTags())
	{
		if (Tag.MatchesTag(NGGameplayTags::Event))
		{
			AbilityTag = Tag;
			break;
		}
	}
	SpecHandle.Data->AddDynamicAssetTag(AbilityTag);
	/////////////////////////////////////////////
	
}
