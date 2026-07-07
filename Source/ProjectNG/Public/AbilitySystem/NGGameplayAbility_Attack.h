// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGGameplayAbility.h"
#include "NGGameplayAbility_Attack.generated.h"

/**
 * 
 */

class UGameplayAbility;

UCLASS()
class PROJECTNG_API UNGGameplayAbility_Attack : public UNGGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	virtual void OnAttackReceived(FGameplayEventData Payload);
	
	UPROPERTY()
	FGameplayEffectSpecHandle SpecHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY()
	FGameplayEventData TossTargetData;

};
