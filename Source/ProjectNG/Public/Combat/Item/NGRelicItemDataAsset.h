// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGItemDataAsset.h"
#include "NGRelicItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGRelicItemDataAsset : public UNGItemDataAsset
{
	GENERATED_BODY()

public:
	virtual UNGItemInstance* CreateInstance(UObject* Outer) override;
};
