// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "Components/NGPathFindingComponent.h"

#include "Combat/Grid/Grid.h"
#include "Pawn/NGPawnBase.h"

UNGPathFindingComponent::UNGPathFindingComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UNGPathFindingComponent::FindPath(const FGridAddress& StartAddress,
                                       const FGridAddress& TargetAddress, TArray<FIntVector2>& OutPath)
{
    // 그리드 맵 유효성 검사
    FGridMapBase* GridMap = UGridMapHelper::GetGridMap(StartAddress);
    if (!GridMap || StartAddress.GridType != TargetAddress.GridType)
    {
        return;
    }

    OutPath.Empty();
    
    FIntVector2 StartIdx = StartAddress.GridIndex;
    FIntVector2 TargetIdx = TargetAddress.GridIndex;

    // A* 알고리즘용 컨테이너
    TArray<FIntVector2> OpenSet;
    TSet<FIntVector2> ClosedSet;
    TMap<FIntVector2, FIntVector2> CameFrom; // 경로 역추적용
    TMap<FIntVector2, int32> GScore; // 시작점으로부터의 실제 비용
    TMap<FIntVector2, int32> FScore; // GScore + 휴리스틱(H) 비용

    OpenSet.Add(StartIdx);
    GScore.Add(StartIdx, 0);
    FScore.Add(StartIdx, GetHeuristicCost(StartIdx, TargetIdx));

    while (OpenSet.Num() > 0)
    {
        // 1. OpenSet에서 FScore가 가장 낮은 노드 찾기
        FIntVector2 CurrentIdx = OpenSet[0];
        int32 LowestF = FScore.Contains(CurrentIdx) ? FScore[CurrentIdx] : MAX_int32;
        int32 CurrentOpenIndex = 0;

        for (int32 i = 1; i < OpenSet.Num(); ++i)
        {
            FIntVector2 TestIdx = OpenSet[i];
            int32 TestF = FScore.Contains(TestIdx) ? FScore[TestIdx] : MAX_int32;
            if (TestF < LowestF)
            {
                LowestF = TestF;
                CurrentIdx = TestIdx;
                CurrentOpenIndex = i;
            }
        }

        // 2. 목적지 도달 시 경로 역추적(Backtracking)
        if (CurrentIdx == TargetIdx)
        {
            FIntVector2 TraceIdx = TargetIdx;
            while (TraceIdx != StartIdx)
            {
                OutPath.Add(TraceIdx); // 배열의 맨 앞에 삽입하여 순서 맞춤
                TraceIdx = CameFrom[TraceIdx];
            }
            
            Algo::Reverse(OutPath);
            return; // 최종 경로 반환
        }

        // 3. 현재 노드를 Open에서 빼고 Closed에 넣음
        OpenSet.RemoveAt(CurrentOpenIndex);
        ClosedSet.Add(CurrentIdx);

        // 4. 인접한 이웃 노드(6방향) 검사
        TArray<FIntVector2> Neighbors;
        UGridMapHelper::GetHexNeighborIndexInRange(CurrentIdx, 1, Neighbors, GridMap);

        for (const FIntVector2& Neighbor : Neighbors)
        {
            // 유효한 인덱스가 아니거나, 이미 닫힌 노드면 무시
            if (!GridMap->IsValidIndex(Neighbor) || ClosedSet.Contains(Neighbor))
                continue;

            // 목적지가 아닌데 타일이 비어있지 않다면(장애물/다른 폰) 갈 수 없음
            if (Neighbor != TargetIdx && !GridMap->CanGoGrid(Neighbor))
                continue;

            // 이동 비용은 인접 타일이므로 무조건 1
            int32 TentativeGScore = GScore[CurrentIdx] + 1;
            int32 NeighborGScore = GScore.Contains(Neighbor) ? GScore[Neighbor] : MAX_int32;

            // 더 빠른 길을 찾았거나 처음 방문하는 노드라면 업데이트
            if (TentativeGScore < NeighborGScore)
            {
                CameFrom.Add(Neighbor, CurrentIdx);
                GScore.Add(Neighbor, TentativeGScore);
                FScore.Add(Neighbor, TentativeGScore + GetHeuristicCost(Neighbor, TargetIdx));

                if (!OpenSet.Contains(Neighbor))
                {
                    OpenSet.Add(Neighbor);
                }
            }
        }
    }

    // 길을 찾지 못함
    return;
}

ANGPawnBase* UNGPathFindingComponent::FindPathToClosestEnemy(const FGridAddress& StartAddress, uint32 OwnerIndex, TArray<FIntVector2>& OutPath)
{
    FGridMapBase* GridMap = UGridMapHelper::GetGridMap(StartAddress);
    if (!GridMap) return nullptr;

    OutPath.Empty();
    
    FIntVector2 StartIdx = StartAddress.GridIndex;

    TArray<FIntVector2> Queue;
    TSet<FIntVector2> Visited;
    TMap<FIntVector2, FIntVector2> CameFrom;
    TMap<FIntVector2, int32> Levels;
    
    Queue.Add(StartIdx);
    Visited.Add(StartIdx);
    Levels.Add(StartIdx, 0);
    
    int32 HeadIndex = 0; 
    int32 FindTimingLevel = -1;
    TArray<ANGPawnBase*> ClosestUnits;
    
    while (HeadIndex < Queue.Num())
    {
        FIntVector2 CurrentIdx = Queue[HeadIndex++]; 
        
        // 현재 꺼낸 노드의 레벨이 적을 발견했던 레벨보다 깊어졌다면
        if (FindTimingLevel != -1 && Levels[CurrentIdx] > FindTimingLevel)
        {
            break; // 여기서 리턴하지 않고 루프를 빠져나가 아래의 공통 결산 로직을 탑니다.
        }
        
        TArray<FIntVector2> Neighbors;
        UGridMapHelper::GetHexNeighborIndexInRange(CurrentIdx, 1, Neighbors, GridMap);
        
        for (const FIntVector2& Neighbor : Neighbors)
        {
            if (!GridMap->IsValidIndex(Neighbor) || Visited.Contains(Neighbor))
                continue;
            
            Visited.Add(Neighbor);
            
            int32 NeighborDataIdx = GridMap->ConvertPointToIndex(Neighbor);
            ANGPawnBase* PlacedPawn = GridMap->GridInfo[NeighborDataIdx].PlacedPawn;
            
            if (PlacedPawn != nullptr)
            {
                if (!PlacedPawn->IsSameTeam(OwnerIndex))
                {
                    CameFrom.Add(Neighbor, CurrentIdx);

                    FindTimingLevel = Levels[CurrentIdx];
                    ClosestUnits.Add(PlacedPawn);
                }
                // 아군이거나 통과 불가 장애물이면 큐에 넣지 않고 지나감
            }
            else
            {
                // 빈 공간일 경우에만 다음 탐색을 위해 큐에 추가
                CameFrom.Add(Neighbor, CurrentIdx);
                Queue.Add(Neighbor);
                Levels.Add(Neighbor, Levels[CurrentIdx] + 1);
            }
        }
    }

    if (ClosestUnits.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, ClosestUnits.Num() - 1);
        ANGPawnBase* ClosestPawn = ClosestUnits[RandomIndex];
                             
        //적 위치는 포함시키지 않기 때문에 한 칸 앞에서부터
        FIntVector2 TraceIdx = CameFrom[ClosestPawn->GetGridAddress().GridIndex];
            
        // 경로 역추적
        while (TraceIdx != StartIdx)
        {
            OutPath.Add(TraceIdx);
            TraceIdx = CameFrom[TraceIdx]; 
        }
            
        Algo::Reverse(OutPath);
        return ClosestPawn;
    }

    // 맵 전체를 뒤졌는데 적이 아예 없는 경우
    return nullptr;
}

int32 UNGPathFindingComponent::GetHeuristicCost(const FIntVector2& A, const FIntVector2& B) const
{
    return UGridMapHelper::GetDistance(A, B);
}