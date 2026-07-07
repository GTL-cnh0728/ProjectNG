// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGGameplayAbility_Attack.h"
#include "NGGameplayAbility_DefaultAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGGameplayAbility_DefaultAttack : public UNGGameplayAbility_Attack
{
	GENERATED_BODY()
protected:
	
	void RegerateMana(const ANGPawnBase* Unit) const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void OnAttackReceived(FGameplayEventData Payload) override;

};
