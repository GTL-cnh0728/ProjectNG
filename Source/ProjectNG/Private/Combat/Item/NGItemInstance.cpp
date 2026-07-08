// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGItemInstance.h"

#include "Net/UnrealNetwork.h"

void UNGItemInstance::InitializeItem(UNGItemDataAsset* InData)
{
	ItemData = InData;
}

void UNGItemInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UNGItemInstance, ItemData);
	DOREPLIFETIME(UNGItemInstance, Count);
}
