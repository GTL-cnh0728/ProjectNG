// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGGameplayAbility_DefaultAttack.h"
#include "NGGameplayAbility_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGGameplayAbility_MeleeAttack : public UNGGameplayAbility_DefaultAttack
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnAttackReceived(FGameplayEventData Payload) override;
};
