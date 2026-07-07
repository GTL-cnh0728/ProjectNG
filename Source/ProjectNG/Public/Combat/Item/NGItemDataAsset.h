// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NGItemDataAsset.generated.h"

class UNGItemInstance;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual UNGItemInstance* CreateInstance(UObject* Outer);
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ItemTag;
	
	UPROPERTY(EditDefaultsOnly)
	FText Name;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> UseGameplayEffect;
};
