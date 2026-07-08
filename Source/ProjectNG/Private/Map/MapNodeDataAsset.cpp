// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "Map/MapNodeDataAsset.h"

bool UMapNodeDataAsset::GetMapNodeVisual(const FGameplayTag& Tag, FMapNodeVisualData& OutData) const
{
	for (const FMapNodeVisualData& Data : MapNodeVisuals)
	{
		if (Data.NodeTag == Tag)
		{
			OutData = Data;
			return true;
		}
	}
	
	return false;
}
