// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NGCombatReward.h"
#include "Engine/DataAsset.h"
#include "NGEnemyDataAsset.generated.h"


class ANGEnemyPawn;

// 몬스터 1마리의 정보
USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TSubclassOf<ANGEnemyPawn> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	FIntVector2 SpawnGridPoint; 
};

// 하나의 CPU 스쿼드
USTRUCT(BlueprintType)
struct FEnemySquadData
{
	GENERATED_BODY()

	bool operator== (const FEnemySquadData& Other) const
	{
		return Other.SquadName == SquadName;
	} 
	
	// 에디터에서 알아보기 쉽게 적을 메모용 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	FName SquadName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	TArray<FEnemySpawnInfo> SpawnUnits;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Squad")
	FReward Reward;
};

// 3. 특정 구역(Zone)에 매핑될 스쿼드 후보 리스트
USTRUCT(BlueprintType)
struct FZoneSquadPool
{
	GENERATED_BODY()

	// 스쿼드 이름 (디버깅용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone Pool")
	FGameplayTag ZoneTag;

	// 이 구역에서 튀어나올 수 있는 CPU 스쿼드 후보 리스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone Pool")
	TArray<FEnemySquadData> SquadCandidates;
};

UCLASS()
class PROJECTNG_API UNGEnemyDataAsset : public UDataAsset
{
	GENERATED_BODY()
    
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy Config")
	TMap<FGameplayTag, FZoneSquadPool> ZoneSquadMasterPool;

	bool GetRandomSquadForZone(const FGameplayTag& TargetZone, FEnemySquadData& OutSquadData) const;
};