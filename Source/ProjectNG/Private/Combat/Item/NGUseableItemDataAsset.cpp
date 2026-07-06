// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGUseableItemDataAsset.h"

#include "Combat/Item/NGUseableItemInstance.h"

UNGItemInstance* UNGUseableItemDataAsset::CreateInstance(UObject* Outer)
{
	return NewObject<UNGUseableItemInstance>(Outer);
}
