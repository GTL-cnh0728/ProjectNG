// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGUserWidget.h"
#include "NGItemSlotWidget.generated.h"

class UNGItemInstance;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGItemSlotWidget : public UNGUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(UNGItemInstance* InItem);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY()
	TObjectPtr<UNGItemInstance> Item;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Icon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ItemCount;
	
	bool bReadyToDrag;
	
};
