// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "Map/NGMapGeneratorComponent.h"
#include "Core/NGGameplayTags.h"
#include "Map/MapNodeDataAsset.h"

UNGMapGeneratorComponent::UNGMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	NodeTypeSpawnRules = {
		{ ENodeType::General, GeneralWeight, 2, 6, 100 },
		{ ENodeType::Combat, CombatWeight, 2, 6, 90 },
		{ ENodeType::Event, EventWeight, 1, 4, 70 },
		{ ENodeType::Shop, ShopWeight, 1, 3, 60 },
		{ ENodeType::Rest, RestWeight, 1, 3, 50 },
		{ ENodeType::Elite, EliteWeight, 1, 2, 40 }
	};
}

void UNGMapGeneratorComponent::GenerateMap(int32 Seed)
{
	RandomSeed = Seed;
	RandomStream.Initialize(RandomSeed);

	UE_LOG(LogTemp, Warning, TEXT("Map Seed: %d"), RandomSeed);

	int32 MaxRetries = 3;
	bool bSuccess = false;

	for (int32 i = 0; i < MaxRetries; ++i)
	{
		GeneratedNodes.Empty();

		GenerateNodes();
		ConnectNodes();

		if (ValidateConnectivity())
		{
			bSuccess = true;
			break;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Map generation validation failed. Retrying... (%d/%d)"), i + 1, MaxRetries);
		}
	}

	if (!bSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate a valid map after %d retries."), MaxRetries);
		return;
	}

	AssignNodeTypes();
	AssignTownBuffs();
	AssignNodePositions();
}

void UNGMapGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
}

FGameplayTag UNGMapGeneratorComponent::GetGameplayTagById(int32 NodeID)
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

TArray<const FMapNodeData*> UNGMapGeneratorComponent::GetNodesAtLayer(int32 LayerIndex) const
{
	TArray<const FMapNodeData*> Result;
	for (const FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == LayerIndex)
		{
			Result.Add(&Node);
		}
	}
	return Result;
}

TArray<const FMapNodeData*> UNGMapGeneratorComponent::GetTownNodes() const
{
	return GetNodesAtLayer(0);
}

void UNGMapGeneratorComponent::GenerateNodes()
{
	int32 NodeIDCounter = 0;

	// 1. Named Node (Layer 3 - Center)
	FMapNodeData NamedNode;
	NamedNode.NodeID = NodeIDCounter++;
	NamedNode.LayerIndex = 3;
	GeneratedNodes.Add(NamedNode);

	// 2. Ring Nodes (Layer 1 - Circular)
	int32 RingCount = RandomStream.RandRange(MinRingNodes, MaxRingNodes);
	for (int32 i = 0; i < RingCount; ++i)
	{
		FMapNodeData RingNode;
		RingNode.NodeID = NodeIDCounter++;
		RingNode.LayerIndex = 1;
		GeneratedNodes.Add(RingNode);
	}

	// 3. Branch Nodes (Layer 2 - Outward)
	int32 TotalNodes = RandomStream.RandRange(TotalNodeCountMin, TotalNodeCountMax);
	int32 RemainingNodes = TotalNodes - GeneratedNodes.Num() - NumberOfTowns;
	if (RemainingNodes < 0) RemainingNodes = 0;

	for (int32 i = 0; i < RemainingNodes; ++i)
	{
		FMapNodeData BranchNode;
		BranchNode.NodeID = NodeIDCounter++;
		BranchNode.LayerIndex = 2;
		GeneratedNodes.Add(BranchNode);
	}

	// 4. Town Nodes (Layer 0 - Start), attached later to outer branch nodes.
	for (int32 i = 0; i < NumberOfTowns; ++i)
	{
		FMapNodeData TownNode;
		TownNode.NodeID = NodeIDCounter++;
		TownNode.LayerIndex = 0;
		GeneratedNodes.Add(TownNode);
	}
}

void UNGMapGeneratorComponent::ConnectNodes()
{
	TArray<FMapNodeData*> NamedNodes, TownNodes, RingNodes, BranchNodes;
	for (FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == 0) TownNodes.Add(&Node);
		else if (Node.LayerIndex == 1) RingNodes.Add(&Node);
		else if (Node.LayerIndex == 2) BranchNodes.Add(&Node);
		else if (Node.LayerIndex == 3) NamedNodes.Add(&Node);
	}

	if (NamedNodes.IsEmpty() || RingNodes.IsEmpty()) return;

	FMapNodeData* NamedNode = NamedNodes[0];

	// 1. Connect Named to all Ring Nodes
	for (FMapNodeData* RingNode : RingNodes)
	{
		NamedNode->ConnectedNodeIDs.AddUnique(RingNode->NodeID);
		RingNode->ConnectedNodeIDs.AddUnique(NamedNode->NodeID);
	}

	// 2. Connect Ring Nodes in a loop
	for (int32 i = 0; i < RingNodes.Num(); ++i)
	{
		FMapNodeData* Current = RingNodes[i];
		FMapNodeData* Next = RingNodes[(i + 1) % RingNodes.Num()];
		Current->ConnectedNodeIDs.AddUnique(Next->NodeID);
		Next->ConnectedNodeIDs.AddUnique(Current->NodeID);
	}

	// 3. Connect Branch Nodes
	TArray<FMapNodeData*> ActiveNodes = RingNodes; // Nodes that can sprout branches
	TMap<int32, int32> RootRingMap;
	for (FMapNodeData* RingNode : RingNodes)
	{
		RootRingMap.Add(RingNode->NodeID, RingNode->NodeID);
	}

	for (FMapNodeData* BranchNode : BranchNodes)
	{
		if (ActiveNodes.IsEmpty()) break;

		int32 SourceIdx = RandomStream.RandRange(0, ActiveNodes.Num() - 1);
		FMapNodeData* SourceNode = ActiveNodes[SourceIdx];

		BranchNode->ConnectedNodeIDs.AddUnique(SourceNode->NodeID);
		SourceNode->ConnectedNodeIDs.AddUnique(BranchNode->NodeID);

		if (RootRingMap.Contains(SourceNode->NodeID))
		{
			RootRingMap.Add(BranchNode->NodeID, RootRingMap[SourceNode->NodeID]);
		}

		ActiveNodes.Add(BranchNode);

		if (SourceNode->ConnectedNodeIDs.Num() >= MaxConnectionsPerNode)
		{
			ActiveNodes.RemoveAt(SourceIdx);
		}
	}

	// 4. Connect leaf nodes to each other occasionally (neighboring subtrees only to avoid cross-center lines)
	TArray<FMapNodeData*> LeafNodes;
	for (FMapNodeData* BranchNode : BranchNodes)
	{
		if (BranchNode->ConnectedNodeIDs.Num() == 1)
		{
			LeafNodes.Add(BranchNode);
		}
	}

	// Sort leaf nodes by their root Ring Node index to keep connections local
	LeafNodes.Sort([&RootRingMap, &RingNodes](const FMapNodeData& A, const FMapNodeData& B) {
		int32 RootA = RootRingMap.Contains(A.NodeID) ? RootRingMap[A.NodeID] : -1;
		int32 RootB = RootRingMap.Contains(B.NodeID) ? RootRingMap[B.NodeID] : -1;

		int32 IndexA = RingNodes.IndexOfByPredicate([RootA](const FMapNodeData* Node) { return Node->NodeID == RootA; });
		int32 IndexB = RingNodes.IndexOfByPredicate([RootB](const FMapNodeData* Node) { return Node->NodeID == RootB; });

		return IndexA < IndexB;
	});

	for (int32 i = 0; i < LeafNodes.Num() - 1; ++i)
	{
		if (RandomStream.FRand() < LeafConnectionProbability)
		{
			LeafNodes[i]->ConnectedNodeIDs.AddUnique(LeafNodes[i+1]->NodeID);
			LeafNodes[i+1]->ConnectedNodeIDs.AddUnique(LeafNodes[i]->NodeID);

			// Increment i to avoid chaining multiple connections to the same leaf
			i++;
		}
	}

	// 5. Attach Town nodes to outer branch leaves so town lines point outward.
	TArray<FMapNodeData*> TownParentCandidates;
	for (FMapNodeData* BranchNode : BranchNodes)
	{
		if (BranchNode->ConnectedNodeIDs.Num() <= 1)
		{
			TownParentCandidates.Add(BranchNode);
		}
	}

	if (TownParentCandidates.Num() < TownNodes.Num())
	{
		for (FMapNodeData* BranchNode : BranchNodes)
		{
			TownParentCandidates.AddUnique(BranchNode);
		}
	}

	if (TownParentCandidates.IsEmpty())
	{
		TownParentCandidates = RingNodes;
	}

	TownParentCandidates.Sort([&RootRingMap, &RingNodes](const FMapNodeData& A, const FMapNodeData& B) {
		int32 RootA = RootRingMap.Contains(A.NodeID) ? RootRingMap[A.NodeID] : A.NodeID;
		int32 RootB = RootRingMap.Contains(B.NodeID) ? RootRingMap[B.NodeID] : B.NodeID;

		int32 IndexA = RingNodes.IndexOfByPredicate([RootA](const FMapNodeData* Node) { return Node->NodeID == RootA; });
		int32 IndexB = RingNodes.IndexOfByPredicate([RootB](const FMapNodeData* Node) { return Node->NodeID == RootB; });

		return IndexA < IndexB;
	});

	for (int32 i = TownParentCandidates.Num() - 1; i > 0; --i)
	{
		int32 SwapIndex = RandomStream.RandRange(0, i);
		TownParentCandidates.Swap(i, SwapIndex);
	}

	for (int32 i = 0; i < TownNodes.Num(); ++i)
	{
		FMapNodeData* TownNode = TownNodes[i];
		FMapNodeData* ParentNode = TownParentCandidates[i % TownParentCandidates.Num()];

		TownNode->ConnectedNodeIDs.AddUnique(ParentNode->NodeID);
		ParentNode->ConnectedNodeIDs.AddUnique(TownNode->NodeID);
	}
}

bool UNGMapGeneratorComponent::ValidateConnectivity()
{
	if (GeneratedNodes.IsEmpty()) return false;

	TArray<int32> Queue;
	TSet<int32> Visited;

	Queue.Add(GeneratedNodes[0].NodeID);
	Visited.Add(GeneratedNodes[0].NodeID);

	while (Queue.Num() > 0)
	{
		int32 CurrentID = Queue[0];
		Queue.RemoveAt(0);

		const FMapNodeData* CurrentNode = nullptr;
		for (const FMapNodeData& Node : GeneratedNodes)
		{
			if (Node.NodeID == CurrentID)
			{
				CurrentNode = &Node;
				break;
			}
		}

		if (!CurrentNode) continue;

		for (int32 ConnectedID : CurrentNode->ConnectedNodeIDs)
		{
			if (!Visited.Contains(ConnectedID))
			{
				Visited.Add(ConnectedID);
				Queue.Add(ConnectedID);
			}
		}
	}

	return Visited.Num() == GeneratedNodes.Num();
}

TArray<FNGNodeTypeSpawnRule> UNGMapGeneratorComponent::GetEffectiveNodeTypeSpawnRules() const
{
	TArray<FNGNodeTypeSpawnRule> Rules = NodeTypeSpawnRules;

	if (Rules.IsEmpty())
	{
		Rules = {
			{ ENodeType::General, GeneralWeight, 0, TNumericLimits<int32>::Max(), 100 },
			{ ENodeType::Combat, CombatWeight, 0, TNumericLimits<int32>::Max(), 90 },
			{ ENodeType::Event, EventWeight, 0, TNumericLimits<int32>::Max(), 70 },
			{ ENodeType::Shop, ShopWeight, 0, TNumericLimits<int32>::Max(), 60 },
			{ ENodeType::Rest, RestWeight, 0, TNumericLimits<int32>::Max(), 50 },
			{ ENodeType::Elite, EliteWeight, 0, TNumericLimits<int32>::Max(), 40 }
		};
	}

	Rules.RemoveAll([](const FNGNodeTypeSpawnRule& Rule)
	{
		return Rule.NodeType == ENodeType::None
			|| Rule.NodeType == ENodeType::Town
			|| Rule.NodeType == ENodeType::Named;
	});

	TSet<ENodeType> SeenTypes;
	Rules.RemoveAll([&SeenTypes](const FNGNodeTypeSpawnRule& Rule)
	{
		if (SeenTypes.Contains(Rule.NodeType))
		{
			return true;
		}

		SeenTypes.Add(Rule.NodeType);
		return false;
	});

	for (FNGNodeTypeSpawnRule& Rule : Rules)
	{
		Rule.Weight = FMath::Max(0.0f, Rule.Weight);
		Rule.MinCount = FMath::Max(0, Rule.MinCount);
		Rule.MaxCount = FMath::Max(0, Rule.MaxCount);

		if (Rule.MaxCount < Rule.MinCount)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Node spawn rule for type %d has MaxCount lower than MinCount. MaxCount was raised to MinCount."),
				static_cast<int32>(Rule.NodeType));
			Rule.MaxCount = Rule.MinCount;
		}
	}

	Rules.StableSort([](const FNGNodeTypeSpawnRule& A, const FNGNodeTypeSpawnRule& B)
	{
		if (A.FillPriority != B.FillPriority)
		{
			return A.FillPriority > B.FillPriority;
		}

		if (!FMath::IsNearlyEqual(A.Weight, B.Weight))
		{
			return A.Weight > B.Weight;
		}

		return static_cast<uint8>(A.NodeType) < static_cast<uint8>(B.NodeType);
	});

	return Rules;
}

TMap<ENodeType, int32> UNGMapGeneratorComponent::BuildNodeTypeCounts(int32 AssignableNodeCount) const
{
	TMap<ENodeType, int32> Counts;
	if (AssignableNodeCount <= 0)
	{
		return Counts;
	}

	const TArray<FNGNodeTypeSpawnRule> Rules = GetEffectiveNodeTypeSpawnRules();
	if (Rules.IsEmpty())
	{
		Counts.Add(ENodeType::General, AssignableNodeCount);
		return Counts;
	}

	float TotalWeight = 0.0f;
	for (const FNGNodeTypeSpawnRule& Rule : Rules)
	{
		TotalWeight += Rule.Weight;
		Counts.FindOrAdd(Rule.NodeType) = 0;
	}

	if (TotalWeight <= KINDA_SMALL_NUMBER)
	{
		Counts.FindOrAdd(Rules[0].NodeType) = AssignableNodeCount;
		return Counts;
	}

	struct FNodeCountRemainder
	{
		ENodeType NodeType = ENodeType::None;
		float Remainder = 0.0f;
		int32 RuleIndex = 0;
	};

	TArray<FNodeCountRemainder> Remainders;
	int32 CurrentTotal = 0;

	for (int32 RuleIndex = 0; RuleIndex < Rules.Num(); ++RuleIndex)
	{
		const FNGNodeTypeSpawnRule& Rule = Rules[RuleIndex];
		const float ExactCount = (Rule.Weight / TotalWeight) * AssignableNodeCount;
		int32 Count = FMath::FloorToInt(ExactCount);
		Count = FMath::Clamp(Count, Rule.MinCount, Rule.MaxCount);

		Counts.FindOrAdd(Rule.NodeType) = Count;
		CurrentTotal += Count;

		Remainders.Add({ Rule.NodeType, ExactCount - FMath::FloorToFloat(ExactCount), RuleIndex });
	}

	Remainders.StableSort([](const FNodeCountRemainder& A, const FNodeCountRemainder& B)
	{
		if (!FMath::IsNearlyEqual(A.Remainder, B.Remainder))
		{
			return A.Remainder > B.Remainder;
		}

		return A.RuleIndex < B.RuleIndex;
	});

	while (CurrentTotal < AssignableNodeCount)
	{
		bool bAdded = false;

		for (const FNodeCountRemainder& Remainder : Remainders)
		{
			const FNGNodeTypeSpawnRule& Rule = Rules[Remainder.RuleIndex];
			int32& Count = Counts.FindOrAdd(Rule.NodeType);

			if (Count < Rule.MaxCount)
			{
				++Count;
				++CurrentTotal;
				bAdded = true;
				break;
			}
		}

		if (!bAdded)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Node spawn rules could not fill %d nodes within MaxCount limits. Filling with %s."),
				AssignableNodeCount - CurrentTotal, *StaticEnum<ENodeType>()->GetNameStringByValue(static_cast<int64>(Rules[0].NodeType)));
			Counts.FindOrAdd(Rules[0].NodeType) += AssignableNodeCount - CurrentTotal;
			CurrentTotal = AssignableNodeCount;
		}
	}

	TArray<int32> TrimOrder;
	for (int32 RuleIndex = Rules.Num() - 1; RuleIndex >= 0; --RuleIndex)
	{
		TrimOrder.Add(RuleIndex);
	}

	while (CurrentTotal > AssignableNodeCount)
	{
		bool bRemoved = false;

		for (int32 RuleIndex : TrimOrder)
		{
			const FNGNodeTypeSpawnRule& Rule = Rules[RuleIndex];
			int32& Count = Counts.FindOrAdd(Rule.NodeType);

			if (Count > Rule.MinCount)
			{
				--Count;
				--CurrentTotal;
				bRemoved = true;
				break;
			}
		}

		if (!bRemoved)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Node spawn rule MinCount total exceeds assignable node count. Reducing lowest priority minimum."));

			for (int32 RuleIndex : TrimOrder)
			{
				const FNGNodeTypeSpawnRule& Rule = Rules[RuleIndex];
				int32& Count = Counts.FindOrAdd(Rule.NodeType);

				if (Count > 0)
				{
					--Count;
					--CurrentTotal;
					bRemoved = true;
					break;
				}
			}

			if (!bRemoved)
			{
				break;
			}
		}
	}

	return Counts;
}

TArray<ENodeType> UNGMapGeneratorComponent::BuildShuffledNodeTypePool(const TMap<ENodeType, int32>& NodeTypeCounts)
{
	TArray<ENodeType> TypePool;
	const TArray<FNGNodeTypeSpawnRule> Rules = GetEffectiveNodeTypeSpawnRules();

	for (const FNGNodeTypeSpawnRule& Rule : Rules)
	{
		const int32* Count = NodeTypeCounts.Find(Rule.NodeType);
		if (!Count)
		{
			continue;
		}

		for (int32 i = 0; i < *Count; ++i)
		{
			TypePool.Add(Rule.NodeType);
		}
	}

	for (int32 i = TypePool.Num() - 1; i > 0; --i)
	{
		const int32 SwapIndex = RandomStream.RandRange(0, i);
		TypePool.Swap(i, SwapIndex);
	}

	return TypePool;
}

FGameplayTag UNGMapGeneratorComponent::GetTagForNodeType(ENodeType Type)
{
	switch (Type)
	{
		case ENodeType::Town:
			return (RandomStream.FRand() < 0.5f) ? NGGameplayTags::Node_Town_Elf : NGGameplayTags::Node_Town_Dwarf;
		case ENodeType::General: return NGGameplayTags::Node_General;
		case ENodeType::Shop:    return NGGameplayTags::Node_Shop;
		case ENodeType::Combat:  return NGGameplayTags::Node_Combat;
		case ENodeType::Event:   return NGGameplayTags::Node_Event;
		case ENodeType::Rest:    return NGGameplayTags::Node_Rest;
		case ENodeType::Elite:   return NGGameplayTags::Node_Elite;
		case ENodeType::Named:   return NGGameplayTags::Node_Named;
		default:                 return FGameplayTag();
	}
}

void UNGMapGeneratorComponent::AssignNodeTypes()
{
	TArray<FMapNodeData*> AssignableNodes;

	for (FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == 0)
		{
			Node.NodeType = ENodeType::Town;
		}
		else if (Node.LayerIndex == 3)
		{
			Node.NodeType = ENodeType::Named;
		}
		else
		{
			AssignableNodes.Add(&Node);
		}
	}

	const TMap<ENodeType, int32> NodeTypeCounts = BuildNodeTypeCounts(AssignableNodes.Num());
	const TArray<ENodeType> TypePool = BuildShuffledNodeTypePool(NodeTypeCounts);

	for (int32 i = 0; i < AssignableNodes.Num(); ++i)
	{
		AssignableNodes[i]->NodeType = TypePool.IsValidIndex(i) ? TypePool[i] : ENodeType::General;
	}

	for (FMapNodeData& Node : GeneratedNodes)
	{
		Node.NodeTag = GetTagForNodeType(Node.NodeType);
	}
}

void UNGMapGeneratorComponent::AssignTownBuffs()
{
	TArray<FGameplayTag> AvailableBuffs = {
		NGGameplayTags::TownBuff_ExtraGold,
		NGGameplayTags::TownBuff_AttackBoost,
		NGGameplayTags::TownBuff_DefenseBoost,
		NGGameplayTags::TownBuff_StartUnit
	};

	for (int32 i = AvailableBuffs.Num() - 1; i > 0; --i)
	{
		int32 j = RandomStream.RandRange(0, i);
		AvailableBuffs.Swap(i, j);
	}

	int32 BuffIndex = 0;
	for (FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == 0 && Node.NodeType == ENodeType::Town)
		{
			if (AvailableBuffs.IsValidIndex(BuffIndex))
			{
				Node.TownBuffTag = AvailableBuffs[BuffIndex++];
			}
		}
	}
}

void UNGMapGeneratorComponent::AssignNodePositions()
{
	TMap<int32, FMapNodeData*> NodeMap;
	for (FMapNodeData& Node : GeneratedNodes)
	{
		NodeMap.Add(Node.NodeID, &Node);
		if (Node.LayerIndex == 3) // Named
		{
			Node.Location = FVector(0.5f, 0.5f, 0.0f);
		}
	}

	TArray<FMapNodeData*> RingNodes, BranchNodes, TownNodes;
	for (FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == 1) RingNodes.Add(&Node);
		else if (Node.LayerIndex == 2) BranchNodes.Add(&Node);
		else if (Node.LayerIndex == 0) TownNodes.Add(&Node);
	}

	TMap<int32, float> NodeAngles;
	TMap<int32, float> NodeRadii;

	for (int32 i = 0; i < RingNodes.Num(); ++i)
	{
		float Angle = (static_cast<float>(i) / RingNodes.Num()) * 2.0f * PI;
		float Radius = 0.24f;
		RingNodes[i]->Location = FVector(0.5f + FMath::Cos(Angle) * Radius, 0.5f + FMath::Sin(Angle) * Radius, 0.0f);
		NodeAngles.Add(RingNodes[i]->NodeID, Angle);
		NodeRadii.Add(RingNodes[i]->NodeID, Radius);
	}

	// BFS to position Branch nodes cleanly radiating outward
	TSet<int32> Visited;
	TArray<int32> Queue;

	for (FMapNodeData* RingNode : RingNodes)
	{
		Visited.Add(RingNode->NodeID);
		Queue.Add(RingNode->NodeID);
	}

	for (FMapNodeData& Node : GeneratedNodes)
	{
		if (Node.LayerIndex == 3)
		{
			Visited.Add(Node.NodeID);
		}
	}

	TMap<int32, int32> NodeDepth;
	for (FMapNodeData* RingNode : RingNodes)
	{
		NodeDepth.Add(RingNode->NodeID, 0);
	}

	while (Queue.Num() > 0)
	{
		int32 CurrentID = Queue[0];
		Queue.RemoveAt(0);

		FMapNodeData* CurrentNode = NodeMap[CurrentID];
		float ParentAngle = NodeAngles[CurrentID];
		float ParentRadius = NodeRadii[CurrentID];
		int32 CurrentDepth = NodeDepth[CurrentID];

		TArray<int32> Children;
		for (int32 NeighborID : CurrentNode->ConnectedNodeIDs)
		{
			if (NodeMap.Contains(NeighborID) && NodeMap[NeighborID]->LayerIndex == 2 && !Visited.Contains(NeighborID))
			{
				Children.Add(NeighborID);
			}
		}

		if (Children.Num() > 0)
		{
			float SpreadAngle = 0.55f / (CurrentDepth + 1);

			for (int32 i = 0; i < Children.Num(); ++i)
			{
				int32 ChildID = Children[i];
				FMapNodeData* ChildNode = NodeMap[ChildID];

				float Fraction = (Children.Num() > 1) ? (static_cast<float>(i) / (Children.Num() - 1)) - 0.5f : 0.0f;
				float AngleOffset = Fraction * SpreadAngle;

				float JitterAngle = RandomStream.FRandRange(-0.035f, 0.035f);
				float JitterRadius = RandomStream.FRandRange(-0.015f, 0.015f);

				float ChildAngle = ParentAngle + AngleOffset + JitterAngle;
				float ChildRadius = ParentRadius + 0.105f + JitterRadius;

				ChildRadius = FMath::Min(ChildRadius, 0.46f);

				ChildNode->Location = FVector(0.5f + FMath::Cos(ChildAngle) * ChildRadius, 0.5f + FMath::Sin(ChildAngle) * ChildRadius, 0.0f);

				NodeAngles.Add(ChildID, ChildAngle);
				NodeRadii.Add(ChildID, ChildRadius);
				NodeDepth.Add(ChildID, CurrentDepth + 1);

				Visited.Add(ChildID);
				Queue.Add(ChildID);
			}
		}
	}

	for (FMapNodeData* BranchNode : BranchNodes)
	{
		if (!NodeAngles.Contains(BranchNode->NodeID))
		{
			float Angle = RandomStream.FRandRange(0.0f, 2.0f * PI);
			float Radius = RandomStream.FRandRange(0.28f, 0.42f);
			BranchNode->Location = FVector(0.5f + FMath::Cos(Angle) * Radius, 0.5f + FMath::Sin(Angle) * Radius, 0.0f);
			NodeAngles.Add(BranchNode->NodeID, Angle);
			NodeRadii.Add(BranchNode->NodeID, Radius);
		}
	}

	for (int32 i = 0; i < TownNodes.Num(); ++i)
	{
		FMapNodeData* TownNode = TownNodes[i];
		int32 ParentID = -1;
		for (int32 ConnectedID : TownNode->ConnectedNodeIDs)
		{
			if (NodeMap.Contains(ConnectedID))
			{
				ParentID = ConnectedID;
				break;
			}
		}

		float Angle = (static_cast<float>(i) / FMath::Max(1, TownNodes.Num())) * 2.0f * PI + (PI / FMath::Max(1, TownNodes.Num()));
		float Radius = 0.5f;
		if (ParentID != -1 && NodeAngles.Contains(ParentID) && NodeRadii.Contains(ParentID))
		{
			Angle = NodeAngles[ParentID] + RandomStream.FRandRange(-0.08f, 0.08f);
			Radius = FMath::Clamp(NodeRadii[ParentID] + 0.11f, 0.44f, 0.52f);
		}

		TownNode->Location = FVector(0.5f + FMath::Cos(Angle) * Radius, 0.5f + FMath::Sin(Angle) * Radius, 0.0f);
		NodeAngles.Add(TownNode->NodeID, Angle);
		NodeRadii.Add(TownNode->NodeID, Radius);
	}

	ResolveNodePositionOverlaps();
	AdjustLongConnectedNodeDistances();
	ResolveNodePositionOverlaps();
}

void UNGMapGeneratorComponent::ResolveNodePositionOverlaps()
{
	constexpr float MinDistance = 0.105f;
	constexpr float MinDistanceSq = MinDistance * MinDistance;
	constexpr float MinCoord = 0.07f;
	constexpr float MaxCoord = 0.93f;
	constexpr int32 IterationCount = 28;

	for (int32 Iteration = 0; Iteration < IterationCount; ++Iteration)
	{
		bool bMovedAny = false;

		for (int32 i = 0; i < GeneratedNodes.Num(); ++i)
		{
			for (int32 j = i + 1; j < GeneratedNodes.Num(); ++j)
			{
				FMapNodeData& A = GeneratedNodes[i];
				FMapNodeData& B = GeneratedNodes[j];

				if (A.LayerIndex == 3 || B.LayerIndex == 3)
				{
					continue;
				}

				FVector2D PosA(A.Location.X, A.Location.Y);
				FVector2D PosB(B.Location.X, B.Location.Y);
				FVector2D Delta = PosB - PosA;
				float DistanceSq = Delta.SizeSquared();

				if (DistanceSq >= MinDistanceSq)
				{
					continue;
				}

				if (DistanceSq <= KINDA_SMALL_NUMBER)
				{
					const float Angle = RandomStream.FRandRange(0.0f, 2.0f * PI);
					Delta = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle));
					DistanceSq = 1.0f;
				}

				const float Distance = FMath::Sqrt(DistanceSq);
				const FVector2D Direction = Delta / Distance;
				const float PushAmount = (MinDistance - Distance) * 0.5f;

				PosA -= Direction * PushAmount;
				A.Location.X = FMath::Clamp(PosA.X, MinCoord, MaxCoord);
				A.Location.Y = FMath::Clamp(PosA.Y, MinCoord, MaxCoord);

				PosB += Direction * PushAmount;
				B.Location.X = FMath::Clamp(PosB.X, MinCoord, MaxCoord);
				B.Location.Y = FMath::Clamp(PosB.Y, MinCoord, MaxCoord);

				bMovedAny = true;
			}
		}

		if (!bMovedAny)
		{
			break;
		}
	}
}

void UNGMapGeneratorComponent::AdjustLongConnectedNodeDistances()
{
	constexpr float MaxEdgeDistance = 0.24f;
	constexpr float MaxEdgeDistanceSq = MaxEdgeDistance * MaxEdgeDistance;
	constexpr float MinCoord = 0.07f;
	constexpr float MaxCoord = 0.93f;
	constexpr int32 IterationCount = 10;

	auto GetMovePriority = [](const FMapNodeData& Node) -> int32
	{
		if (Node.LayerIndex == 0) return 3; // Town
		if (Node.LayerIndex == 2) return 2; // Branch
		if (Node.LayerIndex == 1) return 1; // Ring
		return 0;                           // Named
	};

	for (int32 Iteration = 0; Iteration < IterationCount; ++Iteration)
	{
		bool bMovedAny = false;

		for (FMapNodeData& Node : GeneratedNodes)
		{
			for (int32 ConnectedID : Node.ConnectedNodeIDs)
			{
				if (Node.NodeID >= ConnectedID)
				{
					continue;
				}

				FMapNodeData* ConnectedNode = GeneratedNodes.FindByPredicate([ConnectedID](const FMapNodeData& Candidate) {
					return Candidate.NodeID == ConnectedID;
				});

				if (!ConnectedNode)
				{
					continue;
				}

				FVector2D PosA(Node.Location.X, Node.Location.Y);
				FVector2D PosB(ConnectedNode->Location.X, ConnectedNode->Location.Y);
				FVector2D Delta = PosB - PosA;
				float DistanceSq = Delta.SizeSquared();

				if (DistanceSq <= MaxEdgeDistanceSq || DistanceSq <= KINDA_SMALL_NUMBER)
				{
					continue;
				}

				const float Distance = FMath::Sqrt(DistanceSq);
				const FVector2D Direction = Delta / Distance;
				const float PullAmount = (Distance - MaxEdgeDistance) * 0.65f;
				const int32 PriorityA = GetMovePriority(Node);
				const int32 PriorityB = GetMovePriority(*ConnectedNode);

				if (PriorityA == 0 && PriorityB == 0)
				{
					continue;
				}

				if (PriorityA > PriorityB)
				{
					PosA += Direction * PullAmount;
				}
				else if (PriorityB > PriorityA)
				{
					PosB -= Direction * PullAmount;
				}
				else
				{
					PosA += Direction * PullAmount * 0.5f;
					PosB -= Direction * PullAmount * 0.5f;
				}

				if (PriorityA > 0)
				{
					Node.Location.X = FMath::Clamp(PosA.X, MinCoord, MaxCoord);
					Node.Location.Y = FMath::Clamp(PosA.Y, MinCoord, MaxCoord);
				}

				if (PriorityB > 0)
				{
					ConnectedNode->Location.X = FMath::Clamp(PosB.X, MinCoord, MaxCoord);
					ConnectedNode->Location.Y = FMath::Clamp(PosB.Y, MinCoord, MaxCoord);
				}

				bMovedAny = true;
			}
		}

		if (!bMovedAny)
		{
			break;
		}
	}
}
