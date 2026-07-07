// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/JobSkill/NGGameplayAbility_JobSkill.h"

#include "Kismet/GameplayStatics.h"

void UNGGameplayAbility_JobSkill::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UE_LOG(LogTemp, Log, TEXT("UNGGameplayAbility_JobSkill::ActivateAbility"));
	
	// UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	
	if(ANGPawnBase* Unit = GetNGPawnFromActorInfo())
	{
		Unit->SetAttackCheckTimer(false);
	}
}

void UNGGameplayAbility_JobSkill::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UE_LOG(LogTemp, Log, TEXT("UNGGameplayAbility_JobSkill::EndAbility"));
	// UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	if (ANGPawnBase* Unit = GetNGPawnFromActorInfo())
	{
		Unit->SetAttackCheckTimer(true);
	}

}
