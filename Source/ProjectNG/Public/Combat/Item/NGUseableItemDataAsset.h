// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGItemDataAsset.h"
#include "NGUseableItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGUseableItemDataAsset : public UNGItemDataAsset
{
	GENERATED_BODY()
	
public:
	virtual UNGItemInstance* CreateInstance(UObject* Outer) override;
};
