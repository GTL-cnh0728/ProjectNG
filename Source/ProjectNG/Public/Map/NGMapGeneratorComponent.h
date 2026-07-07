// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Map/NGMapTypes.h"
#include "NGMapGeneratorComponent.generated.h"

class UMapNodeDataAsset;
class ANGMapNode;

USTRUCT(BlueprintType)
struct FNGNodeTypeSpawnRule
{
	GENERATED_BODY()

	FNGNodeTypeSpawnRule() = default;

	FNGNodeTypeSpawnRule(ENodeType InNodeType, float InWeight, int32 InMinCount, int32 InMaxCount, int32 InFillPriority)
		: NodeType(InNodeType)
		, Weight(InWeight)
		, MinCount(InMinCount)
		, MaxCount(InMaxCount)
		, FillPriority(InFillPriority)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|NodeSpawnRules")
	ENodeType NodeType = ENodeType::General;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|NodeSpawnRules", meta = (ClampMin = "0.0"))
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|NodeSpawnRules", meta = (ClampMin = "0"))
	int32 MinCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|NodeSpawnRules", meta = (ClampMin = "0"))
	int32 MaxCount = 999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map|NodeSpawnRules")
	int32 FillPriority = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UNGMapGeneratorComponent();

	// ─── 공개 API ───
	UFUNCTION(BlueprintCallable, Category = "Map")
	void GenerateMap(int32 Seed);
	
	UFUNCTION(BlueprintCallable, Category = "Map")
	FGameplayTag GetGameplayTagById(int32 NodeID);
	
	const TArray<FMapNodeData>& GetGeneratedNodes() const { return GeneratedNodes; };

	// ─── 유틸리티 ───
	// 특정 레이어의 모든 노드 가져오기
	TArray<const FMapNodeData*> GetNodesAtLayer(int32 LayerIndex) const;
	// 시작 마을 노드들만 가져오기
	TArray<const FMapNodeData*> GetTownNodes() const;

protected:
	virtual void BeginPlay() override;

private:
	// ─── 생성 파이프라인 ───
	void GenerateNodes();             // Step 1: 맵 전체 노드 생성
	void ConnectNodes();              // Step 2: 원형 그래프 및 나뭇가지 연결
	bool ValidateConnectivity();      // Step 3: 도달성 검증 (BFS)
	void AssignNodeTypes();           // Step 4: 노드 타입 가중치 배정
	void AssignTownBuffs();           // Step 5: 마을 버프 랜덤 배정
	void AssignNodePositions();       // Step 6: UI용 논리 좌표 배정

	// ─── 헬퍼 ───
	void ResolveNodePositionOverlaps();
	void AdjustLongConnectedNodeDistances();

	TArray<FNGNodeTypeSpawnRule> GetEffectiveNodeTypeSpawnRules() const;
	TMap<ENodeType, int32> BuildNodeTypeCounts(int32 AssignableNodeCount) const;
	TArray<ENodeType> BuildShuffledNodeTypePool(const TMap<ENodeType, int32>& NodeTypeCounts);
	FGameplayTag GetTagForNodeType(ENodeType Type);

public:
	// ─── 데이터 에셋 ───
	UPROPERTY(EditAnywhere, Category = "Map|Data")
	TObjectPtr<UMapNodeDataAsset> MapNodeData;

	// ─── 맵 크기 설정 ───
	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 TotalNodeCountMin = 15;

	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 TotalNodeCountMax = 25;

	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 MinRingNodes = 6;

	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 MaxRingNodes = 8;

	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 NumberOfTowns = 3;         // 시작 마을 수 (고정)

	// ─── 연결 설정 ───
	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 MinConnectionsPerNode = 2;

	UPROPERTY(EditAnywhere, Category = "Map|Config")
	int32 MaxConnectionsPerNode = 4;
	
	// ─── 리프 노드 연결 확률 ───
	UPROPERTY(EditAnywhere, Category = "Map|Config")
	float LeafConnectionProbability = 0.3f;

	// ─── 노드 타입 가중치 ───
	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float GeneralWeight = 0.30f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float ShopWeight = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float CombatWeight = 0.20f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float EventWeight = 0.20f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float RestWeight = 0.10f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeWeights")
	float EliteWeight = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Map|NodeSpawnRules")
	TArray<FNGNodeTypeSpawnRule> NodeTypeSpawnRules;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Map")
	int32 RandomSeed;

private:
	TArray<FMapNodeData> GeneratedNodes;
	FRandomStream RandomStream;
};
