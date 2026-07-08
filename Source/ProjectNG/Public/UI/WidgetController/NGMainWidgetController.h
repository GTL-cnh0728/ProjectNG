// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/NGGameState.h"
#include "UI/NGWidgetController.h"
#include "NGMainWidgetController.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnGameFlowChangedDelegate, EGameplayPhase, CurrentPhase, int32,
                                               CurrentTurn, float, PhaseStartServerTime, float, PhaseDuration, float, RemainingTime);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTNG_API UNGMainWidgetController : public UNGWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Game|Turn")
	FOnGameFlowChangedDelegate OnGameFlowChanged;
	
	
private:
	
	UFUNCTION()
	void HandleGameFlowChanged(EGameplayPhase CurrentPhase, int32 CurrentTurn, float PhaseStartServerTime, float PhaseDuration, float RemainingTime);
};
