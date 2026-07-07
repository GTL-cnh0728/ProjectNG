// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGRelicItemDataAsset.h"

#include "Combat/Item/NGRelicItemInstance.h"

UNGItemInstance* UNGRelicItemDataAsset::CreateInstance(UObject* Outer)
{
	return NewObject<UNGRelicItemInstance>(Outer);
}
