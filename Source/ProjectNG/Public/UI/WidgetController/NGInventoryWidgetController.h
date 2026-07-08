// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/NGWidgetController.h"
#include "NGInventoryWidgetController.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowInventoryWidgetDelegate, bool, bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshInventory);


UCLASS(BlueprintType, Blueprintable)
class PROJECTNG_API UNGInventoryWidgetController : public UNGWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	
	UFUNCTION(BlueprintCallable, Category = "Game|Inventory")
	void ShowInventoryWidget(bool bVisible);
	
	UFUNCTION(BlueprintCallable, Category = "Game|Inventory")
	void RefreshInventory();

		
	UPROPERTY(BlueprintAssignable, Category = "Game|Inventory")
	FOnShowInventoryWidgetDelegate OnShowInventory;
	
	UPROPERTY(BlueprintAssignable, Category = "Game|Inventory")
	FOnRefreshInventory OnRefreshInventory;
	
};
