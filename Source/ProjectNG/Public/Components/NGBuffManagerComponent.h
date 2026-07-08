// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayEffect.h"
#include "Components/ActorComponent.h"
#include "NGBuffManagerComponent.generated.h"


class ANGPawnBase;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FActiveTeamBuff
{
	GENERATED_BODY()
	
public:
	bool operator==(const FActiveTeamBuff& Other) const
	{
		return GameplayEffect == Other.GameplayEffect;
	}
	
	UPROPERTY()
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	UPROPERTY()
	TMap<TObjectPtr<ANGPawnBase>, FActiveGameplayEffectHandle> Handles;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGBuffManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNGBuffManagerComponent();
	bool ApplyExistAllBuffsToUnit(ANGPawnBase* Unit);

	bool AddBuff(TSubclassOf<UGameplayEffect> GameplayEffect);
	
	bool RemoveBuff(TSubclassOf<UGameplayEffect> GameplayEffect);
	
	void ApplyAllBuffsToPawn(ANGPawnBase* Pawn);
	
private:
	UPROPERTY()
	TArray<FActiveTeamBuff> ActiveBuffs;
	
};
