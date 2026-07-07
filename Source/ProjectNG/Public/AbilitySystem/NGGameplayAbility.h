// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Pawn/NGPawnBase.h"
#include "NGGameplayAbility.generated.h"

class ANGCharacterBase;
class ANGUnitPawn;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UNGGameplayAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	ANGPawnBase* GetNGPawnFromActorInfo() const;
	
	UFUNCTION()
	void OnMontageFinished();
};
