// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NGItemDB.generated.h"

class UNGItemDataAsset;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGItemDB : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UNGItemDataAsset>> Items;
};
