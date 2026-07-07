// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "GameModes/NGInGameMode.h"

#include "Components/NGCombatManagerComponent.h"
#include "Core/NGDeveloperSettings.h"
#include "Core/NGUnitData.h"
#include "Game/NGGameState.h"
#include "Game/NGPawnDataManager.h"
#include "Pawn/NGUnitPawn.h"
#include "Player/NGPlayerState.h"


ANGInGameMode::ANGInGameMode()
{
	CombatManagerComponent = CreateDefaultSubobject<UNGCombatManagerComponent>(TEXT("CombatManager"));
}

void ANGInGameMode::RequestStartCombat(APlayerController* PC, bool bIsCPUCombat)
{
	// TODO: 테스트용으로 잠시 막음
	// if (CurrentState == EGameState::Combat)	return;
	
	ANGGameState* GS = GetGameState<ANGGameState>();
		
	// 테스트용
	if (GS)
	{
		for (APlayerState* RawPS : GS->PlayerArray)
		{
			if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
			{
				if (bIsCPUCombat)
				{
					TSoftObjectPtr<UNGEnemyDataAsset> SoftPath = GetDefault<UNGDeveloperSettings>()->EnemyDataAsset;

					if (SoftPath.IsNull()) return;

					UNGEnemyDataAsset* LoadedAsset = SoftPath.LoadSynchronous();
	    
					if (LoadedAsset)
					{
						FEnemySquadData SelectedData;
						if (LoadedAsset->GetRandomSquadForZone(PS->GetCurrentZoneTag(), SelectedData))
						{
							CombatManagerComponent->EnqueueCombatPhase(PS, &SelectedData);
						}
					}
				}
				else
				{
					CombatManagerComponent->EnqueueCombatPhase(PS);
				}
			}
		}
		
		CombatManagerComponent->MatchingCombatUser(true);
		CombatManagerComponent->StartCountingCombat();
	}
}

void ANGInGameMode::OnGameStart()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	NotifyGameStartToPlayer(GS);

	if (GS)
	{
		GS->CurrentTurn = 0;
		StartTurn();
	}
}

void ANGInGameMode::NotifyGameStartToPlayer(ANGGameState* GS)
{
	if (!GS)	return;
	
	for (APlayerState* RawPS : GS->PlayerArray)
	{
		if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
		{
			PS->SetGameState(EGameState::Exploration);
		}
	}	
}

void ANGInGameMode::OnCombatFinished(const FCombatResultData& ResultData)
{
	UE_LOG(LogTemp, Log, TEXT("Combat Finished"));
		
	//ResultData로 점수나 그런거 반영하기
	
	// 각 플레이어가 전투 끝났음을 표시
	CheckAllPlayersFinishedAction();
}

void ANGInGameMode::ReportPawnDeath(ANGPawnBase* DeadPawn) const
{
	if (CombatManagerComponent)
	{
		CombatManagerComponent->NotifyPawnDied(DeadPawn);
	}
}

void ANGInGameMode::StartTurn()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	GS->CurrentTurn++;
	if (GS->CurrentTurn > 10)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max turns (10) reached. Ending game."));
		// TODO: Game Over 로직
		return;
	}

	for (APlayerState* RawPS : GS->PlayerArray)
	{
		if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
		{
			PS->SetHasSelectedNode(false);
			PS->SetActionFinished(false);
		}
	}

	StartNodeSelection();
}

void ANGInGameMode::StartNodeSelection()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	GS->CurrentPhase = EGameplayPhase::NodeSelection;
	GS->RemainingTime = 15.f;

	GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ThisClass::OnNodeSelectionTimerTick, 1.0f, true);
	UE_LOG(LogTemp, Warning, TEXT("=== Turn %d: Node Selection Phase Started (15s) ==="), GS->CurrentTurn);
}

void ANGInGameMode::OnNodeSelectionTimerTick()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;
	
	if (GS->RemainingTime > 0.f)
	{
		GS->RemainingTime -= 1.f;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
		StartActionPhase();
	}
}

void ANGInGameMode::ProcessNodeSelection(AController* Controller, int32 NodeID)
{
	ANGPlayerState* PS = Controller->GetPlayerState<ANGPlayerState>();
	if (PS && !PS->HasSelectedNode())
	{
		PS->SetTargetNodeID(NodeID);
		PS->SetHasSelectedNode(true);
		
		CheckAllPlayersReadyForNodeSelection();
	}
}

void ANGInGameMode::CheckAllPlayersReadyForNodeSelection()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	for (APlayerState* RawPS : GS->PlayerArray)
	{
		if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
		{
			if (!PS->HasSelectedNode())
			{
				return;
			}
		}
	}
	
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	StartActionPhase();
}

void ANGInGameMode::StartActionPhase()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	GS->CurrentPhase = EGameplayPhase::ActionPhase;
	GS->RemainingTime = 60.f;

	UE_LOG(LogTemp, Warning, TEXT("=== Action Phase Started (60s) ==="));

	TArray<ANGPlayerState*> PlayingPlayers;
	for (APlayerState* RawPS : GS->PlayerArray)
	{
		if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
		{
			PlayingPlayers.Add(PS);
		}
	}

	TArray<ANGPlayerState*> HandledPlayers;

	// 거리 계산 로직 (거리 <= 1칸)
	// 현재 맵 제네레이터 정보가 없으므로 ID 차이가 1 이하인 경우로 임시 판별 (이후 ConnectedNodes 로직으로 대체 필요)
	for (int32 i = 0; i < PlayingPlayers.Num(); ++i)
	{
		ANGPlayerState* PlayerA = PlayingPlayers[i];
		if (HandledPlayers.Contains(PlayerA)) continue;

		for (int32 j = i + 1; j < PlayingPlayers.Num(); ++j)
		{
			ANGPlayerState* PlayerB = PlayingPlayers[j];
			if (HandledPlayers.Contains(PlayerB)) continue;

			// 임시 인접 판별 로직
			bool bIsAdjacent = FMath::Abs(PlayerA->GetTargetNodeID() - PlayerB->GetTargetNodeID()) <= 1;

			if (bIsAdjacent)
			{
				UE_LOG(LogTemp, Warning, TEXT("PvP Matched: Player %s vs Player %s"), *PlayerA->GetPlayerName(), *PlayerB->GetPlayerName());
				CombatManagerComponent->EnqueueCombatPhase(PlayerA);
				CombatManagerComponent->EnqueueCombatPhase(PlayerB);
				HandledPlayers.Add(PlayerA);
				HandledPlayers.Add(PlayerB);
				break;
			}
		}
	}

	// 1:1:1 등 홀수 상황에서 남은 유저는 CPU 전투(복제 유닛 전투) 진행
	for (ANGPlayerState* PS : PlayingPlayers)
	{
		if (!HandledPlayers.Contains(PS))
		{
			UE_LOG(LogTemp, Warning, TEXT("CPU Match (Clone): Player %s"), *PS->GetPlayerName());
			// 원래는 1vs1 대상의 복제 유닛 데이터를 Enqueue 해야 함. 임시로 CPU전 세팅
			TSoftObjectPtr<UNGEnemyDataAsset> SoftPath = GetDefault<UNGDeveloperSettings>()->EnemyDataAsset;
			UNGEnemyDataAsset* LoadedAsset = SoftPath.LoadSynchronous();
			if (LoadedAsset)
			{
				FEnemySquadData SelectedData;
				if (LoadedAsset->GetRandomSquadForZone(PS->GetCurrentZoneTag(), SelectedData))
				{
					CombatManagerComponent->EnqueueCombatPhase(PS, &SelectedData);
				}
			}
			HandledPlayers.Add(PS);
		}
	}

	// 노드 이동 적용
	for (ANGPlayerState* PS : PlayingPlayers)
	{
		PS->SetCurrentNodeID(PS->GetTargetNodeID());
	}

	CombatManagerComponent->MatchingCombatUser(false);
	CombatManagerComponent->StartCountingCombat();

	GetWorldTimerManager().SetTimer(PhaseTimerHandle, this, &ThisClass::OnActionPhaseTimerTick, 1.0f, true);
}

void ANGInGameMode::OnActionPhaseTimerTick()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;
	
	if (GS->RemainingTime > 0.f)
	{
		GS->RemainingTime -= 1.f;
	}
	else
	{
		GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
		EndTurn();
	}
}

void ANGInGameMode::CheckAllPlayersFinishedAction()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	for (APlayerState* RawPS : GS->PlayerArray)
	{
		if (ANGPlayerState* PS = Cast<ANGPlayerState>(RawPS))
		{
			if (!PS->IsActionFinished())
			{
				return;
			}
		}
	}
	
	GetWorldTimerManager().ClearTimer(PhaseTimerHandle);
	EndTurn();
}

void ANGInGameMode::EndTurn()
{
	ANGGameState* GS = GetGameState<ANGGameState>();
	if (!GS) return;

	GS->CurrentPhase = EGameplayPhase::TurnEnd;
	UE_LOG(LogTemp, Warning, TEXT("=== Turn %d Ended ==="), GS->CurrentTurn);
	
	StartTurn();
}

void ANGInGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Only server
	InitializeUnitPool();
}

int32 ANGInGameMode::SellUnit(ANGUnitPawn* Unit)
{
	int32 UnitSellValue = 1;
	
	//DataTable참조해서 유닛의 가격이랑 성보고 돈 계산 후 반환
	
	return UnitSellValue;
}

int32 ANGInGameMode::GrabUnitFromPool(FGameplayTag UnitTag)
{
	if (!HasAuthority()) return -1;
	
	if (int32* Count = UnitPool.Find(UnitTag))
	{
		if (*Count > 0)
		{
			(*Count)--;
			return *Count;
		}
	}
	return -1;
}

bool ANGInGameMode::IsExistUnit(FGameplayTag UnitTag)
{
	if (int32* Count = UnitPool.Find(UnitTag))
	{
		return *Count > 0;
	}
	return false;
}

bool ANGInGameMode::IsExistUnitDataTable() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNGPawnDataManager* UnitDataManager = GI->GetSubsystem<UNGPawnDataManager>())
		{
			return UnitDataManager->IsExistUnitDataTable();
		}
	}
	return false;
}

void ANGInGameMode::ReturnUnitToPool(FGameplayTag UnitTag, int32 UnitCount)
{
	if (int32* Count = UnitPool.Find(UnitTag))
	{
		(*Count) += UnitCount;
	}
}

FGameplayTag ANGInGameMode::GetRandomUnitByTier(EUnitTier Tier)
{
	if (TieredUnitPool.Contains(Tier))
	{
		TArray<FGameplayTag> AvailableUnits;
		const TArray<FGameplayTag>& UnitsInTier = TieredUnitPool[Tier];

		// 현재 남아 있는 유닛만 필터링
		for (FGameplayTag UnitTag : UnitsInTier)
		{
			if (int32* Count = UnitPool.Find(UnitTag))
			{
				if (*Count > 0)
				{
					AvailableUnits.Add(UnitTag);
				}
			}
		}

		if (AvailableUnits.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, AvailableUnits.Num() - 1);
			return AvailableUnits[RandomIndex];
		}
	}
	return FGameplayTag();
}

TSubclassOf<ANGUnitPawn> ANGInGameMode::GetUnitClass(FGameplayTag UnitTag) const
{
	const FUnitData* FoundRow = GetUnitData(UnitTag);
	if (!FoundRow)	return nullptr;
	
	return FoundRow->UnitClass;
}

float ANGInGameMode::GetUnitPrice(ANGUnitPawn* Unit) const
{
	const FUnitData* UnitData = GetUnitData(Unit->GetIdentificationTag());
	if (!UnitData) return 0.f;
	
	float UnitSellValue = UnitData->Price;
	UE_LOG(LogTemp, Log, TEXT("Price: %f"), UnitSellValue);
	
	return UnitSellValue;
}

bool ANGInGameMode::CanBuyUnit(FGameplayTag UnitTag, float OwnedGold) const
{
	const FUnitData* UnitData = GetUnitData(UnitTag);
	if (!UnitData) return false;
	
	if (UnitData->Price > OwnedGold)	return false;
	
	return true;
}

const FUnitData* ANGInGameMode::GetUnitData(FGameplayTag UnitTag) const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNGPawnDataManager* UnitDataManager = GI->GetSubsystem<UNGPawnDataManager>())
		{
			return UnitDataManager->GetUnitData(UnitTag);
		}
	}
	return nullptr;
}

void ANGInGameMode::InitializeUnitPool()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UNGPawnDataManager* UnitDataManager = GI->GetSubsystem<UNGPawnDataManager>())
		{
			const TMap<FGameplayTag, FUnitData*>& AllUnitData = UnitDataManager->GetAllUnitDataMap();
			for (const auto& Pair : AllUnitData)
			{
				FGameplayTag UnitTag = Pair.Key;
				FUnitData* Row = Pair.Value;

				if (Row && Row->UnitClass)
				{
					// 총 개수 추가
					UnitPool.Add(UnitTag, Row->TotalCountInPool);

					// 등급별로 유닛 클래스 추가
					TieredUnitPool.FindOrAdd(Row->Tier).Add(UnitTag);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Unit Pool Initialized on Server using UnitDataManager."));
		}
	}
}
