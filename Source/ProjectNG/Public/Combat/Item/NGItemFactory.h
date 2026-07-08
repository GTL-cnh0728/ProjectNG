// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NGItemFactory.generated.h"

class UNGItemInstance;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGItemFactory : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static UNGItemInstance* CreateItem(
		UObject* Outer,
		FGameplayTag ItemTag
	);
	
};
