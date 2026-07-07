// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "NGDamageExecutionCalculation.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGDamageExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UNGDamageExecutionCalculation();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};
