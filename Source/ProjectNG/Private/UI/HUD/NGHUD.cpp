// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "UI/HUD/NGHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/NGInventoryComponent.h"
#include "UI/NGInventoryWidget.h"
#include "UI/NGShopControlWidget.h"
#include "UI/NGUnitInfoWidget.h"
#include "UI/NGUserWidget.h"
#include "UI/WidgetController/NGMainWidgetController.h"
#include "UI/WidgetController/NGRollShopWidgetController.h"
#include "UI/WidgetController/NGMapWidgetController.h"

UNGMainWidgetController* ANGHUD::CreateMainWidgetController(const FWidgetParams& WidgetControllerParams)
{
    if (MainWidgetController == nullptr)
    {
        MainWidgetController = NewObject<UNGMainWidgetController>(this, MainWidgetControllerClass);
        MainWidgetController->AssignWidgetControllerParams(WidgetControllerParams);
        MainWidgetController->BindCallbacksToDependencies();
    }
    
    return MainWidgetController;
}

UNGRollShopWidgetController* ANGHUD::CreateRollShopWidgetController(const FWidgetParams& WidgetControllerParams)
{
    if (RollShopWidgetController == nullptr)
    {
       RollShopWidgetController = NewObject<UNGRollShopWidgetController>(this, RollShopWidgetControllerClass);
       RollShopWidgetController->AssignWidgetControllerParams(WidgetControllerParams);
       RollShopWidgetController->BindCallbacksToDependencies();
    }
    
    return RollShopWidgetController;
}

UNGMapWidgetController* ANGHUD::CreateMapWidgetController(const FWidgetParams& WidgetControllerParams)
{
    if (MapWidgetController == nullptr)
    {
        MapWidgetController = NewObject<UNGMapWidgetController>(this, MapWidgetControllerClass);
        MapWidgetController->AssignWidgetControllerParams(WidgetControllerParams);
        MapWidgetController->BindCallbacksToDependencies();
    }
    
    return MapWidgetController;
}

// Todo
// void ANGHUD::ShowInventory(bool bVisible) const
// {
//     if (!MainWidget || !MainWidget->InventoryWidget)   return;
//     
//     ESlateVisibility Visibility = bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;
//
//     MainWidget->InventoryWidget->SetVisibility(Visibility);
// }
//
// void ANGHUD::RefreshInventory()
// {
//     if (!MainWidget || !MainWidget->InventoryWidget)   return;
//     
//     MainWidget->InventoryWidget->RefreshInventory();
// }

void ANGHUD::InitializeHUD(APlayerController* PC, APlayerState* PS)
{
    checkf(MainWidgetClass, TEXT("[HUD] MainWidgetClass not initialized"));
    checkf(RollShopWidgetControllerClass, TEXT("[HUD] RollShopWidgetController not initialized"));

    UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), MainWidgetClass);
    MainWidget = Cast<UNGUserWidget>(Widget);

    const FWidgetParams WidgetParams(PC, PS);
    UNGMainWidgetController* MainWidgetC = CreateMainWidgetController(WidgetParams);
    MainWidget->ConnectWidgetController(MainWidgetC);
    
    MainWidgetC->BroadcastInitialValues();
    
    if (Widget)
    {
        Widget->AddToViewport();
    }
    
    ANGPlayerState* NGPS = CastChecked<ANGPlayerState>(PS);
    
    // Todo
    // if (UNGInventoryComponent* Inven = NGPS ? NGPS->GetPlayerInventory() : nullptr)
    // {
    //     Inven->OnInventoryChanged.AddDynamic(this, &ANGHUD::RefreshInventory);
    // }
}