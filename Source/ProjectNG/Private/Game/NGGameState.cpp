// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Game/NGGameState.h"

#include "Components/NGCombatManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/NGPlayerState.h"

ANGGameState::ANGGameState() : GridMargin(3000.f)
{
}

void ANGGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ANGGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANGGameState, CurrentPhase);
	DOREPLIFETIME(ANGGameState, CurrentTurn);
	DOREPLIFETIME(ANGGameState, RemainingTime);
	DOREPLIFETIME(ANGGameState, PhaseStartServerTime);
	DOREPLIFETIME(ANGGameState, PhaseDuration);
	DOREPLIFETIME(ANGGameState, ActiveMovementPlayer);
	DOREPLIFETIME(ANGGameState, DiceResult);
	DOREPLIFETIME(ANGGameState, ReachableNodeIDs);
	DOREPLIFETIME(ANGGameState, MapNodes);
}

void ANGGameState::SetMapNodes(const TArray<FMapNodeData>& InNodes)
{
	if (HasAuthority())
	{
		MapNodes = InNodes;
		OnRep_MapNodes(); // 서버(로컬)에서도 이벤트 발생을 위해 수동 호출
	}
}

void ANGGameState::OnRep_GameFlow()
{
	BroadcastGameFlowChanged();
}

void ANGGameState::BroadcastGameFlowChanged()
{
	OnGameFlowChanged.Broadcast(CurrentPhase, CurrentTurn, PhaseStartServerTime, PhaseDuration, GetRemainingTimeByServerClock());
}

void ANGGameState::SetGameFlow(EGameplayPhase NewPhase, float NewPhaseDuration)
{
	CurrentPhase = NewPhase;
	PhaseDuration = NewPhaseDuration;
	PhaseStartServerTime = GetServerWorldTimeSeconds();
	RemainingTime = NewPhaseDuration;
	BroadcastGameFlowChanged();
}

float ANGGameState::GetRemainingTimeByServerClock() const
{
	if (PhaseDuration <= 0.f)
	{
		return 0.f;
	}

	const float ElapsedTime = GetServerWorldTimeSeconds() - PhaseStartServerTime;
	return FMath::Clamp(PhaseDuration - ElapsedTime, 0.f, PhaseDuration);
}

float ANGGameState::GetPhasePercentRemaining() const
{
	if (PhaseDuration <= 0.f)
	{
		return 0.f;
	}

	return GetRemainingTimeByServerClock() / PhaseDuration;
}

void ANGGameState::OnRep_MovementTurn()
{
	OnMovementTurnChanged.Broadcast();
}

void ANGGameState::SetMovementTurn(ANGPlayerState* NewActivePlayer, int32 NewDiceResult,
	const TArray<int32>& NewReachableNodeIDs)
{
	ActiveMovementPlayer = NewActivePlayer;
	DiceResult = NewDiceResult;
	ReachableNodeIDs = NewReachableNodeIDs;
	OnMovementTurnChanged.Broadcast();
}

void ANGGameState::OnRep_MapNodes()
{
	OnMapDataReady.Broadcast();
}

uint8 ANGGameState::GetPlayerId(const ANGPlayerState* InPS)
{
	return InPS->GetPlayerId();
}
