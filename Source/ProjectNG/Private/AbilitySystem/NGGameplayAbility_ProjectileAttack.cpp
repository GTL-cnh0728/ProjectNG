// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/NGGameplayAbility_ProjectileAttack.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Pawn/NGPawnBase.h"
#include "Combat/NGProjectile.h"
#include "Core/NGDeveloperSettings.h"
#include "Core/NGPoolSubSystem.h"

UNGGameplayAbility_ProjectileAttack::UNGGameplayAbility_ProjectileAttack()
{
}

void UNGGameplayAbility_ProjectileAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(FName("Event.Attack.Release"))
		);
	WaitEventTask->EventReceived.AddDynamic(this, &UNGGameplayAbility_ProjectileAttack::OnAttackReceived);
	WaitEventTask->ReadyForActivation();
	
}

void UNGGameplayAbility_ProjectileAttack::OnAttackReceived(FGameplayEventData Payload)
{
	Super::OnAttackReceived(Payload);
	
	if (!GetWorld()) return;
	if (!K2_HasAuthority())	return;

	//DamageEffectClass를 Instigator의 DamageEffect를 가져와서 넣어야하는거 아닌가?
	TWeakObjectPtr<AActor> TargetActor = (TossTargetData.TargetData.Num() > 0 && TossTargetData.TargetData.Get(0) && TossTargetData.TargetData.Get(0)->GetActors().Num() > 0) ? TossTargetData.TargetData.Get(0)->GetActors()[0] : nullptr;
	if (TargetActor.IsValid())
	{
		if (ANGPawnBase* NewTarget = Cast<ANGPawnBase>(TargetActor.Get()))
		{
			UNGPoolSubSystem* Pool = GetWorld()->GetSubsystem<UNGPoolSubSystem>();
			
			if (Pool)
			{
				FVector SpawnLocation = GetNGPawnFromActorInfo()->GetActorLocation() + GetNGPawnFromActorInfo()->GetActorForwardVector() * 100.f;
	
				FTransform SpawnTransform = GetNGPawnFromActorInfo()->GetActorTransform();
				SpawnTransform.SetLocation(SpawnLocation);
				
				//TODO: LoadSynchronous는 로딩시 멈춤유발가능성, 대용량로드시 멈춰도될때 전부 로드해놓기
				UClass* ProjectileClass = GetDefault<UNGDeveloperSettings>()->ProjectileClass[ANGProjectile::StaticClass()].LoadSynchronous();
				
				ANGProjectile* Projectile = Pool->AcquireProjectile(ProjectileClass, SpawnTransform, NewTarget);
				UE_LOG(LogTemp, Log, TEXT("Target Detected: %s"), *TargetActor->GetName());
				
				if (Projectile && DamageEffectClass)
				{
					Projectile->SetSpecHandle(SpecHandle);
				}
			}
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("No Target Detected"));
	}
}
