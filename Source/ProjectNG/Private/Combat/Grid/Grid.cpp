// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Grid/Grid.h"

#include "Pawn/NGPawnBase.h"
#include "Player/NGPlayerState.h"

void FGridMapBase::Internal_Initialize(int32 InW, int32 InH, float InCellSize, const FVector& InPivot, EGridType InGridType)
{
    Width = InW;
    Height = InH;
    CellSize = InCellSize;
    Pivot = InPivot;
    GridType = InGridType;
    ResetGrid();
}

bool FGridMapBase::IsValidIndex(const FIntVector2 GridIndex) const
{
    return (GridIndex.X >= 0 && GridIndex.X < Width) && (GridIndex.Y >= 0 && GridIndex.Y < Height);
}

int32 FGridMapBase::ConvertPointToIndex(const FIntVector2 GridIndex) const
{
    return GridIndex.Y * Width + GridIndex.X;
}

FIntVector2 FGridMapBase::ConvertIndexToPoint(const int32 Index) const
{
    return FIntVector2(Index % Width, Index / Width);
}

void FGridMapBase::SetGridData(FIntVector2 GridIndex, const FGridData& GridData)
{
    if (!IsValidIndex(GridIndex)) return;
    GridInfo[ConvertPointToIndex(GridIndex)] = GridData;
}

void FGridMapBase::EmptyGridMap(const FIntVector2& GridIndex)
{	
    if (!IsValidIndex(GridIndex)) return;
    
    int32 Idx = ConvertPointToIndex(GridIndex);
    GridInfo[Idx].Reset();
}

FGridData FGridMapBase::GetGridData(const FIntVector2 GridIndex) const
{
    if (!IsValidIndex(GridIndex)) return FGridData();
    return GridInfo[ConvertPointToIndex(GridIndex)];
}

void FGridMapBase::ResetGrid()
{
    GridInfo.SetNum(Width * Height);

    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            FGridData EmptyData(nullptr);
            FIntVector2 CurrentIdx(x, y);
            GridInfo[ConvertPointToIndex(CurrentIdx)] = EmptyData;
        }
    }
}

bool FGridMapBase::IsGridIndexEmpty(const FIntVector2& GridIndex) const
{
    return !IsValid(GridInfo[ConvertPointToIndex(GridIndex)].PlacedPawn);
}

TOptional<FIntVector2> FGridMapBase::GetEmptyGridIndex() const
{
    for (int32 y = 0; y < Height; ++y)
    {
        for (int32 x = 0; x < Width; ++x)
        {
            if (!IsValid(GridInfo[ConvertPointToIndex(FIntVector2(x, y))].PlacedPawn))
            {
                return FIntVector2(x, y);
            }
        }
    }
    
    return TOptional<FIntVector2>();
}

bool FGridMapBase::CanGoGrid(const FIntVector2& GridIndex) const
{
    if (!IsGridIndexEmpty(GridIndex))   return false;
    if (GridInfo[ConvertPointToIndex(GridIndex)].bAnyoneIsComing) return false;

    return true;
}

FQuadGridMap::FQuadGridMap() { Internal_Initialize(9, 1, 100.f, FVector::ZeroVector, EGridType::Wait); Offset = 50.f; }

void FQuadGridMap::InitializeMap(const int32 InSizeX, const int32 InSizeY, const float InCellSize, const FVector& InPivot)
{
    Offset = InCellSize / 2.0f;
    Internal_Initialize(InSizeX, InSizeY, InCellSize, InPivot, EGridType::Wait);
}

FHexGridMap::FHexGridMap() { Internal_Initialize(8, 8, 100.f, FVector::ZeroVector, EGridType::Combat); }

void FHexGridMap::InitializeMap(int32 InSizeQ, int32 InSizeR, float InCellSize, const FVector& InPivot)
{
    Internal_Initialize(InSizeQ, InSizeR, InCellSize, InPivot, EGridType::Combat);
}

FVector UGridMapHelper::GetRelativeLocation(FGridAddress GridAddress)
{
    if (FGridMapBase* GridMap = GetGridMap(GridAddress))
    {
        switch (GridAddress.GridType)
        {
        case EGridType::Combat:
            {
                int32 r = GridAddress.GridIndex.Y;
                int32 q = GridAddress.GridIndex.X - FMath::FloorToInt(GridAddress.GridIndex.Y / 2.0f);

                float X = GridMap->CellSize * (3.0f / 2.0f) * r;
                float Y = GridMap->CellSize * FMath::Sqrt(3.0f) * (q + r * 0.5f);

                return FVector(X, Y, 0.0f);
            }
        case EGridType::Wait:
        case EGridType::EnemyWait:
            {
                return FVector(GridAddress.GridIndex.X * GridMap->CellSize + GridMap->Offset, GridAddress.GridIndex.Y * GridMap->CellSize + GridMap->Offset, 0.0f);
            }
        }
    }
    return FVector::ZeroVector;
}

FVector UGridMapHelper::GetWorldLocation(FGridAddress GridAddress)
{
    if (FGridMapBase* GridMap = GetGridMap(GridAddress))
    {
        return GetRelativeLocation(GridAddress) + GridMap->Pivot;
    }
    
    return FVector::ZeroVector;
}

FIntVector2 UGridMapHelper::GetCellIndex(EGridType GridType, const FVector& Location, ANGPlayerState* PS)
{
    FGridAddress GridAddress(FIntVector2::ZeroValue, GridType, PS, 0);
    if (FGridMapBase* GridMap = GetGridMap(GridAddress))
    {
        switch (GridType)
        {
        case EGridType::Combat:
            {
                FVector RelativePos = Location - GridMap->Pivot + FVector(0.01f, 0.01f, 0.0f);
                float Radius = GridMap->CellSize;

                float r = (2.0f / 3.0f * RelativePos.X) / Radius;
                float q = (1.732051f / 3.0f * RelativePos.Y - 1.0f / 3.0f * RelativePos.X) / Radius;
                float s = -q - r;

                int32 iq = FMath::RoundToInt(q);
                int32 ir = FMath::RoundToInt(r);
                int32 is = FMath::RoundToInt(s);

                float q_diff = FMath::Abs(iq - q);
                float r_diff = FMath::Abs(ir - r);
                float s_diff = FMath::Abs(is - s);

                if (q_diff > r_diff && q_diff > s_diff)      iq = -ir - is;
                else if (r_diff > s_diff)                    ir = -iq - is;

                int32 outX = iq + FMath::FloorToInt(ir / 2.0f);
                int32 outY = ir;

                return FIntVector2(outX, outY);
            }
        case EGridType::Wait:
        case EGridType::EnemyWait:
            {
                return FIntVector2(FMath::FloorToInt((Location.X - GridMap->Pivot.X) / GridMap->CellSize), FMath::FloorToInt((Location.Y - GridMap->Pivot.Y) / GridMap->CellSize));
            }
        }
    }
    
    
    return FIntVector2::ZeroValue;
}

EGridType UGridMapHelper::GetGridType(const FVector& GridLocation, ANGPlayerState* PS)
{
    if (PS)
    {
        const FIntVector2 CombatGridIndex = GetCellIndex(EGridType::Combat, GridLocation, PS);
        FHexGridMap& CombatGridMap = PS->GetCombatGridMap();
        if (CombatGridMap.IsValidIndex(CombatGridIndex))
        {
            return EGridType::Combat;
        }
			
        FQuadGridMap& WaitGridMap = PS->GetWaitGridMap();
        const FIntVector2 WaitGridIndex = GetCellIndex(EGridType::Wait, GridLocation, PS);
        if (WaitGridMap.IsValidIndex(WaitGridIndex))
        {
            return EGridType::Wait;
        }
		
        FQuadGridMap& EnemyWaitGridMap = PS->GetEnemyWaitGridMap();
        const FIntVector2 EnemyWaitGridIndex = GetCellIndex(EGridType::EnemyWait, GridLocation, PS);
        if (EnemyWaitGridMap.IsValidIndex(EnemyWaitGridIndex))
        {
            return EGridType::EnemyWait;
        }
    }
	
    return EGridType::None;
    
}

void UGridMapHelper::GetHexNeighborIndexAtExactRange(FIntVector2 MidIndex, int32 Range, TArray<FIntVector2>& OutRingNodes, FGridMapBase* GridMap)
{
    if (!GridMap)   return;
    
    if (Range <= 0)
    {
        //0인 경우는 자기위치
        if (Range == 0)
        {
            OutRingNodes.Add(MidIndex);
        }
        return;
    }
    
    // 링 테두리의 총 타일 개수는 언제나 정확히 (6 * Range)개
    int32 ExpectedSize = 6 * Range;
    OutRingNodes.Empty(ExpectedSize);

    int32 CenterR = MidIndex.Y;
    int32 CenterQ = MidIndex.X - FMath::FloorToInt(MidIndex.Y / 2.0f);

    const int32 DirQ[6] = { 1,  0, -1, -1,  0,  1 };
    const int32 DirR[6] = { 0,  1,  1,  0, -1, -1 };

    int32 CurrentQ = CenterQ + DirQ[4] * Range;
    int32 CurrentR = CenterR + DirR[4] * Range;

    for (int32 i = 0; i < 6; ++i)
    {
        for (int32 j = 0; j < Range; ++j)
        {
            int32 FinalY = CurrentR;
            int32 FinalX = CurrentQ + FMath::FloorToInt(CurrentR / 2.0f);

            FIntVector2 NeighborCoord(FinalX, FinalY);

            if (GridMap->IsValidIndex(NeighborCoord))
            {
                OutRingNodes.Add(NeighborCoord);
            }
            
            CurrentQ += DirQ[i];
            CurrentR += DirR[i];
        }
    }
}

void UGridMapHelper::GetHexNeighborIndexInRange(FIntVector2 MidIndex, int32 Range, TArray<FIntVector2>& OutNeighborNodes, FGridMapBase* GridMap)
{
    if (!GridMap)   return;
    
    int32 ExpectedSize = 1 + 3 * Range * (Range + 1);
    OutNeighborNodes.Empty(ExpectedSize);
    
    TArray<FIntVector2> TempRingNodes;

    //0부터 한다는거는 자기자신 포함
    for (int32 i = 0; i <= Range; ++i)
    {
        GetHexNeighborIndexAtExactRange(MidIndex, i, TempRingNodes, GridMap);
        
        OutNeighborNodes.Append(TempRingNodes);
    }
}

FGridMapBase* UGridMapHelper::GetGridMap(FGridAddress GridAddress)
{
    if (GridAddress.GridOwnerPS)
    {
        switch (GridAddress.GridType)
        {
        case EGridType::Combat:
            {
                return &GridAddress.GridOwnerPS->GetCombatGridMap();
            }
        case EGridType::Wait:
            {
                return &GridAddress.GridOwnerPS->GetWaitGridMap();
            }
        case EGridType::EnemyWait:
            {
                return &GridAddress.GridOwnerPS->GetEnemyWaitGridMap();
            }
        default:
            {
                
                return nullptr;
            }
        }
    }
    UE_LOG(LogTemp, Error, TEXT("UGridMapHelper::GetGridMap - OwnerPS is nullptr!!!"));
    return nullptr;
}

const FIntVector2 UGridMapHelper::GetMirroredIndex(const FGridMapBase& GridMap, FIntVector2 OriginIndex)
{
    return FIntVector2(GridMap.Width - 1 - OriginIndex.X, GridMap.Height - 1 - OriginIndex.Y);
}

int32 UGridMapHelper::GetDistance(FIntVector2 A, FIntVector2 B)
{
    FIntVector ACube = GetCubeIndex(A);
    FIntVector BCube = GetCubeIndex(B);
    return (FMath::Abs(ACube.X - BCube.X) + FMath::Abs(ACube.Y - BCube.Y) + FMath::Abs(ACube.Z - BCube.Z)) / 2;
}

FIntVector2 UGridMapHelper::RectToAxial(int32 Col, int32 Row)
{
    int32 q = Col - FMath::FloorToInt(Row / 2.0f);
    int32 r = Row;
    return FIntVector2(q, r);
}

FIntVector UGridMapHelper::GetCubeIndex(FIntVector2 GridIndex)
{
    int32 X = GridIndex.X - (GridIndex.Y - (GridIndex.Y & 1)) / 2;
    int32 Z = GridIndex.Y;
    int32 Y = -X - Z;

    return FIntVector(X, Y, Z);
}

void UGridMapHelper::DrawDebugGrid(const UObject* WorldContextObject, FGridAddress GridAddress)
{
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return;

    if (FGridMapBase* GridMap = GetGridMap(GridAddress))
    {
        for (int32 i = 0; i < GridMap->GridInfo.Num(); ++i)
        {
            const FGridData& Data = GridMap->GridInfo[i];
            FIntVector2 Idx = GridMap->ConvertIndexToPoint(i);
            GridAddress.GridIndex = Idx;
            
            FVector WorldLoc = GetWorldLocation(GridAddress); // 기존 함수 활용
            
            // 유닛 점유 확인 (PlacedPawn)
            // 가시화: 점유 중이면 빨간 구체, 비어있으면 초록 구체
            FColor DisplayColor = Data.PlacedPawn ? FColor::Red : FColor::Green;
            DrawDebugSphere(World, WorldLoc, 50.f, 12, DisplayColor, false, 0.f, 0, 2.f);
            
            // EmptyGridIndex 포함 여부 확인 (점유 가능 타일)
            // 알고리즘 최적화를 위해 실서비스에선 TSet을 권장하지만, 디버그용이니 Contains도 무방합니다.
            bool bIsEmpty = GridMap->IsGridIndexEmpty(Idx);
            // EmptyGridIndex에 있다면 테두리 표시
            if (bIsEmpty)
            {
                DrawDebugBox(World, WorldLoc, FVector(GridMap->CellSize * 0.4f), FColor::Cyan, false, 0.f, 0, 1.f);
            }

            // 인덱스 텍스트 출력
            DrawDebugString(World, WorldLoc + FVector(0, 0, 20), FString::Printf(TEXT("[%d, %d]"), Idx.X, Idx.Y), nullptr, FColor::White, 0.01f);
        }
        
    }
    
}

