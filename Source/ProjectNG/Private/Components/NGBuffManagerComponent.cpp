// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Components/NGBuffManagerComponent.h"

#include "GameplayEffect.h"
#include "Components/NGPocketComponent.h"
#include "Pawn/NGPawnBase.h"
#include "Player/NGPlayerState.h"


// Sets default values for this component's properties
UNGBuffManagerComponent::UNGBuffManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}

bool UNGBuffManagerComponent::ApplyExistAllBuffsToUnit(ANGPawnBase* Unit)
{
	for (FActiveTeamBuff& Buff : ActiveBuffs)
	{
		UNGAbilitySystemComponent* ASC = Unit->GetNGAbilitySystemComponent();
		
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Buff.GameplayEffect, 1, Context);
		
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		
		Buff.Handles.Add(Unit, Handle);
	}
	
	return true;
}

bool UNGBuffManagerComponent::AddBuff(TSubclassOf<UGameplayEffect> GameplayEffect)
{
	if (!GameplayEffect)	return false;
	
	ANGPlayerState* PS = GetOwner<ANGPlayerState>();
	UNGPocketComponent* Pocket = PS ? PS->GetPlayerPocket() : nullptr;
	
	TArray<ANGPawnBase*> Units = Pocket->GetOwnedUnitPocket();
	
	FActiveTeamBuff NewBuff;
	NewBuff.GameplayEffect = GameplayEffect;
	
	for (ANGPawnBase* Unit : Units)
	{
		if (!Unit)	continue;
		if (!IsValid(Unit))	continue;
		
		UNGAbilitySystemComponent* ASC = Unit->GetNGAbilitySystemComponent();
		if (!ASC)	continue;
		
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(GameplayEffect, 1, Context);
		if (!Spec.IsValid())	continue;
		
		FActiveGameplayEffectHandle Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
		
		NewBuff.Handles.Add(Unit, Handle);
		
	}
	
	ActiveBuffs.Add(NewBuff);
	
	return true;
}

bool UNGBuffManagerComponent::RemoveBuff(TSubclassOf<UGameplayEffect> GameplayEffect)
{
	for (FActiveTeamBuff& Buff : ActiveBuffs)
	{
		if (Buff.GameplayEffect != GameplayEffect)	continue;
		
		for (auto& Pair : Buff.Handles)
		{
			ANGPawnBase* Unit = Pair.Key;
			
			if (!Unit)	continue;
			
			Unit->GetNGAbilitySystemComponent()->RemoveActiveGameplayEffect(Pair.Value);
		}
		ActiveBuffs.RemoveSingle(Buff);
		
		return true;
	}
	
	return false;
}

void UNGBuffManagerComponent::ApplyAllBuffsToPawn(ANGPawnBase* Pawn)
{
	
}
