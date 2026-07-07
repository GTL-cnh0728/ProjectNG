// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Map/NGMapGenerator.h"

#include "Core/NGGameplayTags.h"
#include "Components/SplineMeshComponent.h"
#include "Map/MapNodeDataAsset.h"
#include "Map/NGMapNode.h"

ANGMapGenerator::ANGMapGenerator()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetMobility(EComponentMobility::Static);
}

void ANGMapGenerator::GenerateMap(int32 Seed)
{
	RandomSeed = Seed;
	RandomStream.Initialize(RandomSeed);
	
	UE_LOG(LogTemp, Warning, TEXT("Map Seed: %d"), RandomSeed);
	
	InitializedNodes();
	SpawnNodes();
}

FGameplayTag ANGMapGenerator::GetGameplayTagById(const int32 NodeID)
{
	for (const FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.NodeID == NodeID)
		{
			return Node.NodeTag;
		}
	}
	
	return FGameplayTag();
}

void ANGMapGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	PreInitializeValidPoints();
}

void ANGMapGenerator::InitializedNodes()
{
	GeneratedNodes.Empty();

	if (ValidSpawnPoints.Num() < NumberOfTowns)
	{
		UE_LOG(LogTemp, Error, TEXT("Not enough spawn points for towns! Needed: %d, Found: %d"), NumberOfTowns, ValidSpawnPoints.Num());
		return;
	}

	TArray<FVector2D> CandidatePoints = ValidSpawnPoints;
	TArray<FVector2D> TownPoints;
	TArray<int32> TownIndices;

	// 첫번째 마을 노드 위치 선택
	int32 FirstTownIdx = RandomStream.RandRange(0, CandidatePoints.Num() - 1);
	TownPoints.Add(CandidatePoints[FirstTownIdx]);
	TownIndices.Add(FirstTownIdx);
	
	// Native GameplayTags

	// 최장거리 샘플링을 통해 가장 먼 곳에 새로운 마을 노드 위치 선택.
	while (TownPoints.Num() < NumberOfTowns)
	{
		int32 BestCandidateIdx = -1;
		float MaxMinDistSq = -1.0f;

		for (int32 i = 0; i < CandidatePoints.Num(); ++i)
		{
			if (TownIndices.Contains(i)) continue;

			// 현재 후보인 위치에서 가장 가까운 마을까지의 거리를 구함.
			float MinDistSq = FLT_MAX;
			for (const FVector2D& TownPos : TownPoints)
			{
				float DistSq = FVector2D::DistSquared(CandidatePoints[i], TownPos);
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
				}
			}
			
			// 그 가장 가까운 거리가 가장 먼 후보를 선택
			if (MinDistSq > MaxMinDistSq)
			{
				MaxMinDistSq = MinDistSq;
				BestCandidateIdx = i;
			}
		}

		if (BestCandidateIdx != -1)
		{
			TownPoints.Add(CandidatePoints[BestCandidateIdx]);
			TownIndices.Add(BestCandidateIdx);
		}
		else
		{
			break; // Should not happen if Count < ValidPoints
		}
	}

	// 마을 노드 생성.
	for (int32 i = 0; i < TownPoints.Num(); ++i)
	{
		FMapNodeData NewTown;
		NewTown.NodeID = GeneratedNodes.Num();
		
		if (RandomStream.FRand() < 0.5f)
		{
			NewTown.NodeTag = NGGameplayTags::Node_Town_Dwarf;
		}
		else
		{
			NewTown.NodeTag = NGGameplayTags::Node_Town_Elf;
		}
		
		float WorldX = (TownPoints[i].X * MapRadius) - (MapRadius / 2.0f);
		float WorldY = (TownPoints[i].Y * MapRadius) - (MapRadius / 2.0f);
		NewTown.Location = FVector(WorldX, WorldY, 0.0f);

		GeneratedNodes.Add(NewTown);
	}

	// 다른 노드들 생성을 위해 남은 위치 인덱스 정리.
	TArray<int32> RemainingIndices;
	for(int32 i=0; i<CandidatePoints.Num(); ++i)
	{
		if(!TownIndices.Contains(i))
		{
			RemainingIndices.Add(i);
		}
	}

	// 다양한 위치를 선택하기 위해 인덱스 셔플
	for (int32 i = 0; i < RemainingIndices.Num(); ++i)
	{
		int32 SwapIdx = RandomStream.RandRange(0, RemainingIndices.Num() - 1);
		RemainingIndices.Swap(i, SwapIdx);
	}
	
	const int32 EventNodeCount = FMath::Min(RemainingIndices.Num(), MaxNodeCount);
	
	for (int32 i = 0; i < EventNodeCount; ++i)
	{
		int32 idx = RemainingIndices[i];
		FMapNodeData EventNode;
		EventNode.NodeID = GeneratedNodes.Num();

		float WorldX = (CandidatePoints[idx].X * MapRadius) - (MapRadius / 2.0f);
		float WorldY = (CandidatePoints[idx].Y * MapRadius) - (MapRadius / 2.0f);
		EventNode.Location = FVector(WorldX, WorldY, 0.0f);

		if (RandomStream.FRand() < 0.2f)
		{
			EventNode.NodeTag = NGGameplayTags::Node_Event_Combat;
		}
		else
		{
			EventNode.NodeTag = NGGameplayTags::Node_Event_Default;
		}
		GeneratedNodes.Add(EventNode);
	}
	
	// 노드 연결
	ConnectionNodes();
	
	// 비주얼 업데이트
	DrawPathVisual();
}

void ANGMapGenerator::SpawnNodes()
{
	checkf(MapNodeData, TEXT("Please, choose map data asset."));
	
	for (const FMapNodeData& NodeData : GeneratedNodes)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(NodeData.Location);
		
		TSubclassOf<ANGMapNode> NodeClass = MapNodeData->GetMapNodeData(NodeData.NodeTag);
		if (NodeClass)
		{
			ANGMapNode* NewNode = GetWorld()->SpawnActor<ANGMapNode>(NodeClass, SpawnTransform);
			NewNode->InitializeNode(NodeData);
		}
	}
}

void ANGMapGenerator::PreInitializeValidPoints()
{
	ValidSpawnPoints.Empty();
	if (!IsValid(MapMaskTexture)) return;
	
	FTexture2DMipMap& Mip = MapMaskTexture->GetPlatformData()->Mips[0];
	const FColor* PixelData = static_cast<const FColor*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
	
	int32 Width = MapMaskTexture->GetSizeX();
	int32 Height = MapMaskTexture->GetSizeY();
	
	TArray<bool> Visited;
	Visited.SetNumZeroed(Width * Height);

	// using Connected Component Analysis (Blob Detection)
	for (int32 y = 0; y < Height; ++y)
	{
		for (int32 x = 0; x < Width; ++x)
		{
			int32 Index = y * Width + x;
			if (Visited[Index]) continue;

			FColor PixelColor = PixelData[Index];
			if (PixelColor.R < 10) // 검은색 픽셀을 찾았다면
			{
				// FloodFill 알고리즘을 통해 덩어리 검출
				TArray<FIntPoint> Queue;
				Queue.Add(FIntPoint(x, y));
				Visited[Index] = true;

				double SumX = 0;
				double SumY = 0;
				int32 BlobPixelCount = 0;

				int32 QIndex = 0;
				while(QIndex < Queue.Num())
				{
					FIntPoint P = Queue[QIndex++];
					SumX += P.X;
					SumY += P.Y;
					BlobPixelCount++;

					// 4방향만 검색
					const FIntPoint Neighbors[] = {
						FIntPoint(P.X + 1, P.Y), FIntPoint(P.X - 1, P.Y),
						FIntPoint(P.X, P.Y + 1), FIntPoint(P.X, P.Y - 1)
					};

					for(const FIntPoint& NP : Neighbors)
					{
						if (NP.X >= 0 && NP.X < Width && NP.Y >= 0 && NP.Y < Height)
						{
							int32 NIdx = NP.Y * Width + NP.X;
							if (!Visited[NIdx] && PixelData[NIdx].R < 10)
							{
								Visited[NIdx] = true;
								Queue.Add(NP);
							}
						}
					}
				}

				// 중앙 픽셀 계산
				if (BlobPixelCount > 0)
				{
					float CentroidX = SumX / BlobPixelCount;
					float CentroidY = SumY / BlobPixelCount; // Note: 텍스처의 Y축 위치가 월드 좌표에 대해 반전될 수 있지만, 상대적인 간격 조정에는 문제가 없음.
					
					// Normalize UV
					float U = CentroidX / Width;
					float V = CentroidY / Height;
					ValidSpawnPoints.Add(FVector2D(U, V));
				}
			}
		}
	}
	
	Mip.BulkData.Unlock();
	UE_LOG(LogTemp, Warning, TEXT("Detected Blobs (Nodes): %d"), ValidSpawnPoints.Num());
}

void ANGMapGenerator::ConnectionNodes()
{
	int32 NodeCount = GeneratedNodes.Num();
	
	// KNN Algorithm (K-Nearest Neighbors) + RNG (Relative-Neighborhoods Graph)
	/**
	 * KNN: 무조건 자신과 가장 가까운 노드 K개랑만 연결
	 * RNG: A와 B를 연결하려고 할 때, 둘 사이의 거리보다 A랑도 가깝고 B랑도 가까운 제3의 노드 C가 있다면 연결 X
	 */
	for (int32 i = 0; i < NodeCount; ++i)
	{
		constexpr int32 NearestCount = 3;
		FMapNodeData& SourceNodeData = GeneratedNodes[i];
		TArray<FNodeDistanceInfo> Candidates;
		
		for (int32 j = 0; j < NodeCount; ++j)
		{
			if (i == j) continue;
			
			float DistSq = FVector::DistSquared(SourceNodeData.Location, GeneratedNodes[j].Location);
			Candidates.Add({ j, DistSq });
		}
		
		Candidates.Sort();
		
		int32 ConnectionCount = FMath::Min(Candidates.Num(), NearestCount);
		
		for (int32 k = 0; k < ConnectionCount; ++k)
		{
			int32 TargetIndex = Candidates[k].TargetIndex;
			float TargetDistSq = Candidates[k].DistSquared;
			
			FMapNodeData& TargetNodeData = GeneratedNodes[TargetIndex];
			
			bool bExistBlocker = false;
			
			for (int32 m = 0; m < NodeCount; ++m)
			{
				if (m == i || m == TargetIndex) continue;
				
				float DistToC = FVector::DistSquared(SourceNodeData.Location, GeneratedNodes[m].Location);
				float DistTargetToC = FVector::DistSquared(TargetNodeData.Location, GeneratedNodes[m].Location);
				
				if (DistToC < TargetDistSq && DistTargetToC < TargetDistSq)
				{
					bExistBlocker = true;
					break;
				}
			}
			
			if (!bExistBlocker)
			{
				SourceNodeData.ConnectedNodeIDs.AddUnique(TargetNodeData.NodeID);
				TargetNodeData.ConnectedNodeIDs.AddUnique(SourceNodeData.NodeID);
			}
		}
	}
}

void ANGMapGenerator::DrawPathVisual()
{
	if (!PathMesh) return;
	
	for (const FMapNodeData& SourceNodeData : GeneratedNodes)
	{
		for (int32 TargetID : SourceNodeData.ConnectedNodeIDs)
		{
			// 중복방지 : ID가 작은 쪽에서만 생성
			if (SourceNodeData.NodeID < TargetID)
			{
				const FMapNodeData& TargetNodeData = GeneratedNodes[TargetID];
				USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
				SplineMesh->SetCastShadow(false);
				SplineMesh->SetStaticMesh(PathMesh);
				
				SplineMesh->RegisterComponent();
				if (PathMaterial)
				{
					SplineMesh->SetMaterial(0, PathMaterial);
				}
				
				FVector StartPos = SourceNodeData.Location;
				FVector EndPos = TargetNodeData.Location;
				
				FVector Direction = EndPos - StartPos;
				float Distance = Direction.Size();
				
				float CurveDir = (RandomStream.FRand() > 0.5f) ? 1.0f : -1.0f;
				FVector RightVec = FVector::CrossProduct(Direction, FVector::UpVector).GetSafeNormal();
				
				FVector TangentOffset = RightVec * (Distance * 0.3f) * CurveDir;
				
				SplineMesh->SetStartAndEnd(StartPos, Direction + TangentOffset, EndPos, Direction + TangentOffset);
				SplineMesh->SetStartScale(FVector2D(1.0f, 1.0f));
				SplineMesh->SetEndScale(FVector2D(1.0f, 1.0f));
				SplineMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}


