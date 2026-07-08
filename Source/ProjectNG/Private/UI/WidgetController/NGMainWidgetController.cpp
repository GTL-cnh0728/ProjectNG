// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/WidgetController/NGMainWidgetController.h"

#include "Game/NGGameState.h"
#include "Kismet/GameplayStatics.h"

void UNGMainWidgetController::BroadcastInitialValues()
{
	if (!PlayerController) return;
	
	if (ANGGameState* GS = Cast<ANGGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		OnGameFlowChanged.Broadcast(GS->CurrentPhase, GS->CurrentTurn, GS->PhaseStartServerTime, GS->PhaseDuration, GS->GetRemainingTimeByServerClock());
	}
}

void UNGMainWidgetController::BindCallbacksToDependencies()
{
	if (!PlayerController) return;
	
	if (ANGGameState* GS = Cast<ANGGameState>(UGameplayStatics::GetGameState(PlayerController)))
	{
		GS->OnGameFlowChanged.AddDynamic(this, &UNGMainWidgetController::HandleGameFlowChanged);
	}
}

void UNGMainWidgetController::HandleGameFlowChanged(EGameplayPhase CurrentPhase, int32 CurrentTurn, float PhaseStartServerTime, float PhaseDuration, float RemainingTime)
{
	OnGameFlowChanged.Broadcast(CurrentPhase, CurrentTurn, PhaseStartServerTime, PhaseDuration, RemainingTime);
}
