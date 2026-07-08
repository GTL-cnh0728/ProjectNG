// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Core/NGEnemyDataAsset.h"

bool UNGEnemyDataAsset::GetRandomSquadForZone(const FGameplayTag& TargetZone, FEnemySquadData& OutSquadData) const
{
	if (ZoneSquadMasterPool.Find(TargetZone))
	{
		const FZoneSquadPool& ZonePool = ZoneSquadMasterPool[TargetZone];
		
		if (ZonePool.SquadCandidates.IsEmpty())	return false;
		
		int32 RandomIndex = FMath::RandRange(0, ZonePool.SquadCandidates.Num() - 1);
		OutSquadData = ZonePool.SquadCandidates[RandomIndex];
		return true;
	}

	return false;
}

bool UNGEnemyDataAsset::GetRandomSquadForZoneAndNodeType(const FGameplayTag& TargetZone, ENodeType NodeType,
	FEnemySquadData& OutSquadData) const
{
	const FZoneSquadPool* ZonePool = ZoneSquadMasterPool.Find(TargetZone);
	if (!ZonePool) return false;

	TArray<const FEnemySquadData*> Candidates;
	for (const FEnemySquadData& Squad : ZonePool->SquadCandidates)
	{
		if (Squad.EncounterNodeType == NodeType)
		{
			Candidates.Add(&Squad);
		}
	}

	if (Candidates.IsEmpty()) return false;

	OutSquadData = *Candidates[FMath::RandRange(0, Candidates.Num() - 1)];
	return true;
}
