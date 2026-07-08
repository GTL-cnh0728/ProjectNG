// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/WidgetController/NGInventoryWidgetController.h"

#include "Player/NGPlayerController.h"

void UNGInventoryWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
}

void UNGInventoryWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	
	if (ANGPlayerController* PC = Cast<ANGPlayerController>(PlayerController))
	{
		PC->OnShowInventory.AddDynamic(this, &UNGInventoryWidgetController::ShowInventoryWidget);
	}
}

void UNGInventoryWidgetController::ShowInventoryWidget(bool bVisible)
{
	OnShowInventory.Broadcast(bVisible);
}

void UNGInventoryWidgetController::RefreshInventory()
{
	OnRefreshInventory.Broadcast();
}

// Widget BP에서 구현?
// void ANGHUD::RefreshInventory()
// {
//     if (!MainWidget || !MainWidget->InventoryWidget)   return;
//     
//     MainWidget->InventoryWidget->RefreshInventory();
// }
