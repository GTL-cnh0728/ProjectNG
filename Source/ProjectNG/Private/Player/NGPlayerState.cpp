// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "Player/NGPlayerState.h"

#include "AbilitySystem/NGPlayerAttributeSet.h"
#include "Combat/Grid/Arena.h"
#include "Combat/Grid/ArenaManager.h"
#include "Components/NGPocketComponent.h"
#include "Core/NGDeveloperSettings.h"
#include "Game/NGGameState.h"
#include "GameModes/NGInGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Pawn/NGUnitPawn.h"
#include "Player/NGPlayerController.h"

ANGPlayerState::ANGPlayerState() : CurrentGameState(EGameState::Maintaining), CurrentZoneTag(FGameplayTag::RequestGameplayTag(FName("Zone.Area.A")))
{
	PrimaryActorTick.bCanEverTick = false;
	
	// GAS를 원활히 사용하기 위함
	SetNetUpdateFrequency(100.0f);

	AbilitySystemComponent = CreateDefaultSubobject<UNGAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UNGPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
	
	PlayerPocket = CreateDefaultSubobject<UNGPocketComponent>("PocketComponent");
}

void ANGPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANGPlayerState, CombatGridMap);
	DOREPLIFETIME(ANGPlayerState, WaitGridMap);
	DOREPLIFETIME(ANGPlayerState, EnemyWaitGridMap);
	DOREPLIFETIME(ANGPlayerState, PlayerPocket);
	DOREPLIFETIME(ANGPlayerState, HomeArena);
	DOREPLIFETIME(ANGPlayerState, CurrentGameState);

	DOREPLIFETIME(ANGPlayerState, CurrentNodeID);
	DOREPLIFETIME(ANGPlayerState, TargetNodeID);
	DOREPLIFETIME(ANGPlayerState, bHasSelectedNode);
	DOREPLIFETIME(ANGPlayerState, bIsActionFinished);
	DOREPLIFETIME(ANGPlayerState, bSkipNextMovementTurn);
	DOREPLIFETIME(ANGPlayerState, bSkippedMovementThisTurn);
}

void ANGPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->InitStats(UNGPlayerAttributeSet::StaticClass(), DefaultPlayerAttributeTable);
		}
		
	}
}

void ANGPlayerState::SpawnGridMapManager()
{
	ANGPlayerController* PC = GetOwner<ANGPlayerController>();
		
	FTransform SpawnTransform(FRotator::ZeroRotator, FVector::ZeroVector);
	
	if (ANGGameState* GS = GetWorld()->GetGameState<ANGGameState>())
	{
		int32 UserIndex = GS->GetPlayerId(this);
		FVector Margin(GS->GridMargin * (UserIndex / 4), GS->GridMargin * (UserIndex % 4), 0.f);
		SpawnTransform.AddToTranslation(Margin);
	}
	
	TSoftClassPtr<AArena> GridMapClass = GetDefault<UNGDeveloperSettings>()->ArenaClass;
	if (!GridMapClass.IsNull())
	{
		//SM들을 배정해둬서 BP로 가져와야함
		if (UClass* GridMapManagerBPClass = GridMapClass.LoadSynchronous())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = PC;
			HomeArena = GetWorld()->SpawnActor<AArena>(GridMapManagerBPClass, SpawnTransform, SpawnParams);
			
			if (HomeArena)
			{
				FGridBuildData BuildData(8, 8, 100.f, 1, 9, 150.f);
				HomeArena->Initialize(BuildData, PC->GetPlayerState<ANGPlayerState>());
			}
			
			FActorSpawnParameters ArenaSpawnParams;
			ArenaSpawnParams.Owner = PC ? PC->GetPlayerState<ANGPlayerState>() : nullptr;
			
			ArenaManager = GetWorld()->SpawnActor<AArenaManager>(AArenaManager::StaticClass(), SpawnTransform, ArenaSpawnParams);
			
			if (ArenaManager)
			{
				FArenaAddress ArenaAddress(HomeArena, EPossessArenaIdentification::Home);
				ArenaManager->PossessArena(ArenaAddress);
			}
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("NGDeveloperSettings - Invalid ArenaClass!"));
	}
}

void ANGPlayerState::InitializePostLogin()
{
	SpawnGridMapManager();
	
}

void ANGPlayerState::CaptureSnapShot()
{
	CombatGridMapSnapShot = CombatGridMap;
	
	if (UNGPocketComponent* Pocket = GetPlayerPocket())
	{
		float MaxHP;
		float HP;
		Pocket->CollectTotalUnitHPAndMaxHP(MaxHP, HP);
		Pocket->SetTotalUnitHPSnapShot(HP);
		Pocket->SetTotalUnitMaxHPSnapShot(MaxHP);
	}
}

void ANGPlayerState::OnEnterGameState(const EGameState& NewState)
{
	switch (NewState)
	{
	case EGameState::GameOver:
		{
			UE_LOG(LogTemp, Log, TEXT("GameOver"));
			
			if (UNGPocketComponent* PocketComp = GetPlayerPocket())
			{
				if (ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>())
				{
					TArray<ANGPawnBase*> OwnedPawns = PocketComp->GetOwnedUnitPocket();
					for (ANGPawnBase* Pawn : OwnedPawns)
					{
						GM->ReturnUnitToPool(Pawn->GetIdentificationTag());
						Pawn->Destroy();
					}
					
					OwnedPawns.Empty();
				}
			}
			
			break;
		}
	}
}

void ANGPlayerState::OnExitGameState(const EGameState& PreState)
{
	
}

void ANGPlayerState::SetGameState(EGameState NewState)
{
	OnExitGameState(CurrentGameState);
	CurrentGameState = NewState;
	OnEnterGameState(NewState);
	
}

void ANGPlayerState::EarnGold(float EarnedGold) const
{
	if(!AbilitySystemComponent)	return;

	AbilitySystemComponent->ApplyModToAttribute(
		UNGPlayerAttributeSet::GetGoldAttribute(),
		EGameplayModOp::Additive,
		EarnedGold);
	
	UE_LOG(LogTemp, Log, TEXT("Price: %f New Gold : %f"), EarnedGold, AttributeSet->GetGold());
}

void ANGPlayerState::OnCombatEnd(FCombatResultData CombatResult)
{
	EarnGold(CombatResult.EarnedReward.Gold);
	
	switch (CombatResult.WinResult)
	{
	case ECombatResult::Draw:
		{
			
			break;
		}
	case ECombatResult::Win:
		{
			OnCombatWin();
			break;
		}
	case ECombatResult::Lose:
		{
			OnCombatLose();
			break;
		}
	}
}

float ANGPlayerState::GetOwnedGold() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetGold();
	}
	
	return 0.f;
}

void ANGPlayerState::OnCombatWin()
{
}

void ANGPlayerState::OnCombatLose()
{
	//포켓이 비어있으면 사망처리
	if (UNGPocketComponent* PocketComp = GetPlayerPocket())
	{
		TArray<ANGPawnBase*> PlacedPawns;
		PocketComp->GetPlacedUnits(PlacedPawns);
		
		if (PlacedPawns.IsEmpty())
		{
			SetGameState(EGameState::GameOver);
		}
	}
}

int32 ANGPlayerState::GetUserIndex()
{
	if (ANGGameState* GS = GetWorld()->GetGameState<ANGGameState>())
	{
		return GS->GetPlayerId(this);
	}
	
	return -1;
}

void ANGPlayerState::AddCPUEnemyCount()
{
	if (ANGPlayerController* PC = GetOwner<ANGPlayerController>())
	{
		if (PC->IsLocalController())
		{
			++CurrentCPUEnemyCount;
		}
	}
}

void ANGPlayerState::InitCPUCombat(const FEnemySquadData& SquadData)
{
	CPUEnemyDieCount = 0;
	CurrentCPUEnemyCount = SquadData.SpawnUnits.Num();
}

void ANGPlayerState::RestoreInitialGrid()
{
	if (!HasAuthority())	return;
	
	for (int32 i = 0; i < CombatGridMapSnapShot.GridInfo.Num(); i++)
	{
		FGridData& GridData = CombatGridMapSnapShot.GridInfo[i];

		if (GridData.PlacedPawn)
		{
			FIntVector2 OriginalIndex = CombatGridMapSnapShot.ConvertIndexToPoint(i);

			FGridAddress GridAddress(OriginalIndex, EGridType::Combat, this, GridData.PlacedPawn->GetGridAddress().DirtyFlag);
			
			GridData.PlacedPawn->TransitionToState(EPawnState::None);
			GridData.PlacedPawn->TranslatePawnOnGrid(GridAddress);
		}
	}
	
	for (int32 i = 0; i<EnemyWaitGridMap.GridInfo.Num(); i++)
	{
		FIntVector2 Index = EnemyWaitGridMap.ConvertIndexToPoint(i);
		EnemyWaitGridMap.EmptyGridMap(Index);
	}
}

void ANGPlayerState::StartCombat()
{
	if (UNGPocketComponent* Pocket = GetPlayerPocket())
	{
		TArray<ANGPawnBase*> PlacedPawns;
		Pocket->GetPlacedUnits(PlacedPawns);
		if (PlacedPawns.IsEmpty())
		{
			OnCombatLose();
		}
	}
	
	FHexGridMap& GridMap = GetCombatGridMap();
			
	for (FGridData GridData : GridMap.GridInfo)
	{
		ANGPawnBase* PlacedPawn = GridData.PlacedPawn;
		if (IsValid(PlacedPawn))
		{
			PlacedPawn->TransitionToState(EPawnState::Combat);
		}
	}
}

void ANGPlayerState::FinishCombat()
{
	FHexGridMap& GridMap = GetCombatGridMap();
			
	for (FGridData GridData : GridMap.GridInfo)
	{
		ANGPawnBase* PlacedPawn = GridData.PlacedPawn;
		if (IsValid(PlacedPawn))
		{
			PlacedPawn->TransitionToState(EPawnState::None);
		}
	}
}

