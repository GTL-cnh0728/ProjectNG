// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGEquipmentItemDataAsset.h"

#include "Combat/Item/NGEquipmentItemInstance.h"

UNGItemInstance* UNGEquipmentItemDataAsset::CreateInstance(UObject* Outer)
{
	return NewObject<UNGEquipmentItemInstance>(Outer);
}
