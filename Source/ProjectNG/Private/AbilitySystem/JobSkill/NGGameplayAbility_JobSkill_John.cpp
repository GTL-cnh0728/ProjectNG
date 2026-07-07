// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/JobSkill/NGGameplayAbility_JobSkill_John.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/NGPawnAttributeSet.h"

void UNGGameplayAbility_JobSkill_John::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Event.JobSkill.John")));
	EventTask->EventReceived.AddDynamic(this, &UNGGameplayAbility_JobSkill_John::OnAttackReceived);
	EventTask->ReadyForActivation();
}

void UNGGameplayAbility_JobSkill_John::OnAttackReceived(FGameplayEventData Payload)
{
	ANGPawnBase* OwningActor = GetNGPawnFromActorInfo();
	UNGAbilitySystemComponent* OwnerASC = OwningActor ? OwningActor->GetNGAbilitySystemComponent() : nullptr;
	const UNGPawnAttributeSet* OwnerAttributeSet = OwnerASC ? OwnerASC->GetSet<UNGPawnAttributeSet>() : nullptr;
	
	if (!OwningActor || !OwnerASC || !OwnerAttributeSet)
	{
		K2_EndAbility();
		return;
	}
	
	FGridAddress OwnerGridAddress = OwningActor->GetGridAddress();
	
	if (SpecHandle.IsValid())
	{
		if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(OwnerGridAddress))
		{
			if (GridMap->GridType == EGridType::Combat)
			{
				TArray<FIntVector2> InRangeVectors;
				UGridMapHelper::GetHexNeighborIndexInRange(OwnerGridAddress.GridIndex, OwnerAttributeSet->GetAttackRange(), InRangeVectors, GridMap);
				for (const FIntVector2& Index : InRangeVectors )
				{
					int32 ConvertedIndex = GridMap->ConvertPointToIndex(Index);
					if (ANGPawnBase* TargetPawn = GridMap->GridInfo[ConvertedIndex].PlacedPawn)
					{
						if (!TargetPawn->IsSameTeam(OwningActor->GetOwnerIndex()))
						{
							if (UNGAbilitySystemComponent* TargetASC = TargetPawn->GetNGAbilitySystemComponent())
							{
								OwnerASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
							}
						}
					}
				}
			}
		}
	}
	
	K2_EndAbility();
}