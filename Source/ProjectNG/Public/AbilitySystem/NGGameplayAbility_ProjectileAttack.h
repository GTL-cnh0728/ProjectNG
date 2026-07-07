// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGGameplayAbility.h"
#include "NGGameplayAbility_DefaultAttack.h"
#include "NGGameplayAbility_ProjectileAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGGameplayAbility_ProjectileAttack : public UNGGameplayAbility_DefaultAttack
{
	GENERATED_BODY()
	
public:
	UNGGameplayAbility_ProjectileAttack();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	virtual void OnAttackReceived(FGameplayEventData Payload) override;
};
