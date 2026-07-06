// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGUserWidget.h"
#include "NGShopControlWidget.generated.h"

class UTextBlock;
class UNGWidgetController;
class UNGButtonWidget;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGShopControlWidget : public UNGUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnFleeButtonClicked();
	
	UFUNCTION()
	void OnInvenButtonClicked();
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateGoldText(float NewGold);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGButtonWidget> InvenButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGButtonWidget> FleeButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGInventoryWidget> InventoryWidget;
};
