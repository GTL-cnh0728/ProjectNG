// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "UI/HUD/NGHUD.h"
#include "Blueprint/UserWidget.h"
#include "UI/NGInventoryWidget.h"
#include "UI/NGShopControlWidget.h"
#include "UI/NGUnitInfoWidget.h"
#include "UI/NGUserWidget.h"
#include "UI/WidgetController/NGRollShopWidgetController.h"
#include "UI/WidgetController/UnitDetailsWidgetController.h"

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

UUnitDetailsWidgetController* ANGHUD::CreateUnitDetailsWidgetController(const FWidgetParams& WidgetControllerParams)
{
    if (UnitDetailsWidgetController == nullptr)
    {
        UnitDetailsWidgetController = NewObject<UUnitDetailsWidgetController>(this, UnitDetailsWidgetControllerClass);
        UnitDetailsWidgetController->AssignWidgetControllerParams(WidgetControllerParams);
        UnitDetailsWidgetController->BindCallbacksToDependencies();
        UnitDetailsWidgetController->SetAttributeInfo(AttributeInfo);
    }
    
    return UnitDetailsWidgetController;
}

void ANGHUD::ShowInventory(bool bVisible) const
{
    if (!MainWidget)   return;
    
    ESlateVisibility Visibility = bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

    MainWidget->InventoryWidget->SetVisibility(Visibility);
}

void ANGHUD::InitializeHUD(APlayerController* PC, APlayerState* PS)
{
    checkf(MainWidgetClass, TEXT("[HUD] MainWidgetClass not initialized"));
    checkf(RollShopWidgetControllerClass, TEXT("[HUD] RollShopWidgetController not initialized"));
    checkf(UnitInfoWidgetClass, TEXT("[HUD] UnitInfoWidgetClass not initialized"));
    
    MainWidget = CreateWidget<UNGShopControlWidget>(PC, MainWidgetClass);

    const FWidgetParams WidgetParams(PC, PS);
    UNGRollShopWidgetController* MainWidgetC = CreateRollShopWidgetController(WidgetParams); 
    MainWidget->ConnectWidgetController(MainWidgetC);
    
    MainWidgetC->BroadcastInitialValues();
    
    UnitInfoWidget = CreateWidget<UNGUnitInfoWidget>(PC, UnitInfoWidgetClass);
    
    if (UnitInfoWidget)
    {
        UUnitDetailsWidgetController* UnitDetailsWidgetC = CreateUnitDetailsWidgetController(WidgetParams);
        UnitInfoWidget->ConnectWidgetController(UnitDetailsWidgetC);
        UnitDetailsWidgetC->BroadcastInitialValues();
        
        UnitInfoWidget->AddToViewport();
        UnitInfoWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
    
    if (MainWidget)
    {
        MainWidget->AddToViewport();
    }
}
