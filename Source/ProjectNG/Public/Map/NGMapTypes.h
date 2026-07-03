// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NGMapTypes.generated.h"

// 맵 노드 타입
UENUM(BlueprintType)
enum class ENodeType : uint8
{
	None,
	Town,       // 시작 마을 (3개 고정)
	General,    // 일반 노드 (상점 팝업)
	Shop,       // 상점 노드 (더 많은 선택지)
	Event,      // 이벤트 노드 (랜덤 버프/디버프)
	Rest,       // 휴식 노드 (체력 회복)
	Combat,     // 전투 노드 (중립 몬스터)
	Elite,      // 엘리트 노드 (강한 전투, 높은 보상)
	Named       // 네임드 노드 (최강 보스, 중앙 배치)
};

// 맵의 노드 데이터
USTRUCT(BlueprintType)
struct FMapNodeData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 NodeID = -1; // 식별 ID

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 LayerIndex = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ENodeType NodeType = ENodeType::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTag NodeTag = FGameplayTag();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGameplayTag TownBuffTag = FGameplayTag();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector Location = FVector::ZeroVector; // 월드 좌표
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<int32> ConnectedNodeIDs;
};