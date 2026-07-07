// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Grid.generated.h"

/**
 * 
 */

class ANGPawnBase;
class ANGPlayerState;

UENUM(BlueprintType)
enum class EGridType : uint8 {
	None,
	Combat,
	Wait,
	EnemyWait,
};

USTRUCT(BlueprintType)
struct FGridAddress
{
	GENERATED_BODY()
	
	UPROPERTY()
	FIntVector2 GridIndex;

	UPROPERTY()
	EGridType GridType;
	
	UPROPERTY()
	TObjectPtr<ANGPlayerState> GridOwnerPS;
	
	UPROPERTY()
	uint8 DirtyFlag;
	
	FGridAddress()
		: GridIndex(FIntVector2::ZeroValue)
		, GridType(EGridType::Combat)
		, GridOwnerPS(nullptr)
		, DirtyFlag(0) // 기본값 0 세팅
	{}

	FGridAddress(FIntVector2 InGridIndex, EGridType InGridType, ANGPlayerState* InOwnerPS, uint8 InDirtyFlag)
		: GridIndex(InGridIndex)
		, GridType(InGridType)
		, GridOwnerPS(InOwnerPS)
		, DirtyFlag(InDirtyFlag)
	{}
	
	bool operator==(const FGridAddress& Other) const
	{
		return GridIndex == Other.GridIndex && GridType == Other.GridType && GridOwnerPS == Other.GridOwnerPS;
	}
};

USTRUCT(BlueprintType)
struct FGridData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ANGPawnBase> PlacedPawn;
	
	UPROPERTY()
	bool bAnyoneIsComing;
	
	void Reset()
	{
		PlacedPawn = nullptr;
		bAnyoneIsComing = false;
	}
};

USTRUCT(BlueprintType)
struct PROJECTNG_API FGridMapBase
{
	GENERATED_BODY()

	void Internal_Initialize(int32 InW, int32 InH, float InCellSize, const FVector& InPivot, EGridType InGridType);
	
	bool IsValidIndex(const FIntVector2 GridIndex) const;
	int32 ConvertPointToIndex(const FIntVector2 GridIndex) const;
	FIntVector2 ConvertIndexToPoint(int32 Index) const;

	void SetGridData(FIntVector2 GridIndex, const FGridData& GridData);
	void EmptyGridMap(const FIntVector2& GridIndex);
	FGridData GetGridData(const FIntVector2 GridIndex) const;
    
	void ResetGrid();
    
	bool IsGridIndexEmpty(const FIntVector2& GridIndex) const;
	TOptional<FIntVector2> GetEmptyGridIndex() const;
	bool CanGoGrid(const FIntVector2& GridIndex) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Height;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Pivot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGridType GridType;

	UPROPERTY()
	TArray<FGridData> GridInfo;

	UPROPERTY()
	float Offset;
};

USTRUCT(BlueprintType)
struct PROJECTNG_API FHexGridMap : public FGridMapBase
{
	GENERATED_BODY()

	FHexGridMap();
	
	void InitializeMap(int32 InW, int32 InH, float InCellSize, const FVector& InPivot);
	
};

USTRUCT(BlueprintType)
struct PROJECTNG_API FQuadGridMap : public FGridMapBase
{
	GENERATED_BODY()

	FQuadGridMap();
	void InitializeMap(int32 InW, int32 InH, float InCellSize, const FVector& InPivot);
	
};

UCLASS()
class PROJECTNG_API UGridMapHelper : public UObject
{
	GENERATED_BODY()

public:
	static FVector GetRelativeLocation(FGridAddress GridAddress);
	static FVector GetWorldLocation(FGridAddress GridAddress);
	static FIntVector2 GetCellIndex(EGridType GridType, const FVector& Location, ANGPlayerState* PS);
	static EGridType GetGridType(const FVector& GridLocation, ANGPlayerState* PS);
	
	static void GetHexNeighborIndexInRange(FIntVector2 MidIndex, int32 Range, TArray<FIntVector2>& OutNeighborNodes, FGridMapBase* GridMap);
	static void GetHexNeighborIndexAtExactRange(FIntVector2 MidIndex, int32 Range, TArray<FIntVector2>& OutRingNodes, FGridMapBase* GridMap);
	
	static FGridMapBase* GetGridMap(FGridAddress GridAddress);

	static const FIntVector2 GetMirroredIndex(const FGridMapBase& GridMap, FIntVector2 OriginIndex);
    
	static int32 GetDistance(FIntVector2 A, FIntVector2 B);
	static FIntVector2 RectToAxial(int32 Col, int32 Row);
	static FIntVector GetCubeIndex(const FIntVector2 AxialIndex);
	
	static void DrawDebugGrid(const UObject* WorldContextObject, FGridAddress GridAddress);
};