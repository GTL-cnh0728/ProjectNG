// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGGameplayAbility_JobSkill.h"
#include "NGGameplayAbility_JobSkill_John.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGGameplayAbility_JobSkill_John : public UNGGameplayAbility_JobSkill
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnAttackReceived(FGameplayEventData Payload) override;

};
