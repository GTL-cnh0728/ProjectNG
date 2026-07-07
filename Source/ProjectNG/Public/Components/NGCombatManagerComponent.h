// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/NGCombatData.h"
#include "Core/NGEnemyDataAsset.h"
#include "NGCombatManagerComponent.generated.h"

struct FEnemySquadData;
class ANGPlayerController;
class ANGPawnBase;
class ANGPlayerState;
struct FCombatSettingData;

UCLASS()
class PROJECTNG_API UNGCombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UNGCombatManagerComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void StartCombat();
	void ReturnSpectatorHome(ANGPlayerState* AwayPlayer);
	void NotifyEndCombat(ANGPlayerState* TargetPlayer, ECombatResult Result);
	int32 CalculateTransferPenaltyGold(ANGPlayerState* Loser);

	void NotifyPawnDied(ANGPawnBase* DeadPawn);

	void FinishCombat();
	void ClearCombatEndDatas();

	void ResetGrid(ANGPlayerState* PS);

	void EnqueueCombatPhase(ANGPlayerState* PS, const FEnemySquadData* CPUCombatData = nullptr);
	void StartCountingCombat();
	void MatchingCombatUser(bool bIsShuffle);

	void RequestSpawnSquadByPlayer(ANGPlayerController* RequestingPC, const FEnemySquadData& SquadData) const;
	
	void ProcessPlayerFlee(ANGPlayerController* PlayerController);
	
protected:
	//매개변수 의도된 복사
	void SetupCombat(const FCombatSettingData& SettingData);
	
	UPROPERTY()
	TArray<ANGPlayerState*> CombatPSMatchingQueue;

	UPROPERTY()
	TArray<FCombatSettingData> CombatDatas;
	
	UPROPERTY()
	TMap<ANGPlayerState*, FCombatResultData> CombatResultDictionary;
	
	UPROPERTY()
	int32 FinishedCombatCount;
	
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	float FightWaitTime;
	
	FTimerHandle FightStartTimerHandle;
};
