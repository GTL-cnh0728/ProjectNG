// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "NGItemDragDropOperation.generated.h"

class UNGItemInstance;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UNGItemInstance> DragItem;
};
