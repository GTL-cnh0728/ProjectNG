// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NGUserWidget.h"
#include "NGInventoryWidget.generated.h"

class UNGButtonWidget;
class UUniformGridPanel;
class UNGInventoryComponent;
class UNGItemSlotWidget;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGInventoryWidget : public UNGUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	UNGInventoryComponent* GetInventory();

public:
	UFUNCTION(BlueprintCallable)
	void RefreshInventory();
	
	UFUNCTION(BlueprintCallable)
	void RefreshTabState();
	
	UFUNCTION(BlueprintCallable)
	void ClickTab(FGameplayTag TabTag);
	
protected:
	UPROPERTY()
	TObjectPtr<UNGInventoryComponent> InventoryCache;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> ItemList;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGButtonWidget> ConsumableTabButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGButtonWidget> EquipmentTabButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UNGButtonWidget> RelicTabButton;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNGItemSlotWidget> ItemSlotClass;
	
	UPROPERTY()
	FGameplayTag CurrentTab;
};
