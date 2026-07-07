// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGItemDataAsset.h"

#include "Combat/Item/NGItemInstance.h"

UNGItemInstance* UNGItemDataAsset::CreateInstance(UObject* Outer)
{
	return NewObject<UNGItemInstance>(Outer);
}

