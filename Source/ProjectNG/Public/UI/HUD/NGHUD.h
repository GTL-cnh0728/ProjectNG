// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/NGWidgetController.h"
#include "NGHUD.generated.h"

class UNGInventoryWidgetController;
class UNGMainWidgetController;
class UNGShopControlWidget;
class UNGUnitInfoWidget;
class UUnitDetailsWidgetController;
class UNGUnitAttributeInfoDataAsset;
class UNGRollShopWidgetController;
class UNGMapWidgetController;
class UNGMapScreenWidget;
class UNGUserWidget;

UCLASS()
class PROJECTNG_API ANGHUD : public AHUD
{
    GENERATED_BODY()

public:
    // 🎯 헤더와 cpp의 시그니처를 완전히 일치시킵니다.
    UNGMainWidgetController* CreateMainWidgetController(const FWidgetParams& WidgetControllerParams);
    UNGRollShopWidgetController* CreateRollShopWidgetController(const FWidgetParams& WidgetControllerParams);
    UNGMapWidgetController* CreateMapWidgetController(const FWidgetParams& WidgetControllerParams);
    UNGInventoryWidgetController* CreateInventoryWidgetController(const FWidgetParams& WidgetControllerParams);

    UNGMainWidgetController* GetMainWidgetController() { return MainWidgetController;}
    UNGRollShopWidgetController* GetRollShopWidgetController() { return RollShopWidgetController; }
    UNGMapWidgetController* GetMapWidgetController() { return MapWidgetController; }
    UNGInventoryWidgetController* GetInventoryWidgetController() { return InventoryWidgetController; }
    UNGUnitInfoWidget* GetUnitInfoWidget() { return UnitInfoWidget; }
    
    void InitializeHUD(APlayerController* PC, APlayerState* PS);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WidgetData")
    TObjectPtr<UNGUnitAttributeInfoDataAsset> AttributeInfo;
    
private:
    //~ Begin Main Widget
    UPROPERTY()
    TObjectPtr<UNGUserWidget> MainWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> MainWidgetClass;
    //~ End Main Widget
    
    // ~Begin MainWidgetController
    UPROPERTY()
    TObjectPtr<UNGMainWidgetController> MainWidgetController;
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UNGMainWidgetController> MainWidgetControllerClass;
    // ~End MainWidgetController

    //~ Begin RollShop Widget
    UPROPERTY()
    TObjectPtr<UNGRollShopWidgetController> RollShopWidgetController;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UNGRollShopWidgetController> RollShopWidgetControllerClass;
    //~ End RollShop Widget
    
    //~ Begin UnitDetail Widget
    UPROPERTY()
    TObjectPtr<UNGUnitInfoWidget> UnitInfoWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UNGUnitInfoWidget> UnitInfoWidgetClass; 
    //~ End UnitDetail Widget

    //~ Begin Map WidgetController
    UPROPERTY()
    TObjectPtr<UNGMapWidgetController> MapWidgetController;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UNGMapWidgetController> MapWidgetControllerClass;
    //~ End Map WidgetController
    
    //~ Begin Map WidgetController
    UPROPERTY()
    TObjectPtr<UNGInventoryWidgetController> InventoryWidgetController;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UNGInventoryWidgetController> InventoryWidgetControllerClass;
    //~ End Map WidgetController
};