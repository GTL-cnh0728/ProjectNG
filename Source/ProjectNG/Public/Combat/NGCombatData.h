#pragma once

#include "CoreMinimal.h"
#include "Core/NGEnemyDataAsset.h"
#include "NGCombatData.generated.h"

class ANGPlayerState;
class ANGEnemyPawn;

UENUM(BlueprintType)
enum class ECombatResult : uint8
{
	None = 0,
	Win = 1,
	Lose = 2,
	Draw = 3,
};

USTRUCT()
struct FCombatSettingData
{
	GENERATED_BODY()
	
	FCombatSettingData()
	{
		Players.SetNum(2);
		bIsCPUCombat = false;
	}
	
	void Reset()
	{
		Players.Reset();
		Players.SetNum(2);
		bIsCPUCombat = false;
		EnemySquadData = FEnemySquadData();
	}
	
	UPROPERTY()
	bool bIsCPUCombat;
	
	UPROPERTY()
	TArray<TObjectPtr<ANGPlayerState>> Players;
	
	UPROPERTY()
	FEnemySquadData EnemySquadData;
};

USTRUCT()
struct FCombatResultData
{
	GENERATED_BODY()
	
	FCombatResultData()
	{
		WinResult = ECombatResult::None;
		EarnedReward = FReward(0);
	}
	
	UPROPERTY()
	ECombatResult WinResult;
	
	UPROPERTY()
	FReward EarnedReward;
	//얻은 전리품같은거
};