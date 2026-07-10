// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "NGPlayerState.h"
#include "Map/NGMapTypes.h"
#include "Core/NGEnum.h"
#include "GameFramework/PlayerController.h"
#include "NGPlayerController.generated.h"

class UNGItemInstance;
class ANGUnitPawn;
class ANGHUD;
class AGridMapManager;
class UNGUnitInfoWidget;
class UInputMappingContext;
class UInputAction;
class UNGPocketComponent;

/**
 * 플레이어의 입력을 처리하는 Class
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitsUpdatedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowInventory, bool, bVisible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyUnitSuccessSignature, bool, bIsSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNodeActionStartedSignature, ENodeType, NodeType, int32, NodeID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPvPCombatStartedSignature, ANGPlayerState*, OpponentPlayer, int32, NodeID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatResultSignature, FCombatResultData, CombatResult);

UCLASS()
class PROJECTNG_API ANGPlayerController : public APlayerController
{
	GENERATED_BODY()

	ANGPlayerController();
	~ANGPlayerController();
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;

	/*************************************/
/*				피킹 관련			 */
/*************************************/
public:
	FVector2D GetStartMousePosition() const {return ClickStartLocation;}
	FVector2D GetCurrentMousePosition() const {return CurrentMouseLocation;}
	
	UPROPERTY(BlueprintAssignable, Category = "Game|Shop")
	FOnUnitsUpdatedSignature OnUnitsUpdated;

	virtual void OnRep_PlayerState() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	void PerformDragUpdate(float DeltaTime);
	void ResetHighlight();
	void HighLightGrid(const FVector& TargetLocation, AArena* Arena);
	bool CanHighlight(const FGridAddress& GridAddress) const;

	void SetHoveringUnit(ANGPawnBase* InHoveringPawn);
	void ClearHoveringUnit();
	ANGPawnBase* GetHoveringUnit() const; 
	
	void SetDragItemWithUpdateUI(UNGItemInstance* InItem);
	void SetDragItem(UNGItemInstance* InItem);
	void OnItemDragReleased();

protected:

	void HandleClickPressed(const FInputActionValue& Value);
	void HandleClickReleased(const FInputActionValue& Value);
	
	void SetSelectedUnit(ANGPawnBase* InSelectedUnit);
	void ResetSelectUnit();
	
	void PerformDrag();
	void ResetDragUnit();
	void ResetHoveringUnit();

	// Enhanced Input 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ClickInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MousePositionInputAction;

	uint8 GridMapIndex;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop", meta=(AllowPrivateAccess = "true"))
	float DragThreshold;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop", meta=(AllowPrivateAccess = "true"))
	float DragHeightOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop", meta=(AllowPrivateAccess = "true"))
	float DragInterpSpeed;
	
	FVector2D ClickStartLocation;
	FVector2D CurrentMouseLocation;

	FVector CurrentHighlightLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	TWeakObjectPtr<ANGPawnBase> HoveringUnit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	TWeakObjectPtr<ANGPawnBase> DraggingUnit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	TObjectPtr<ANGPawnBase> SelectedUnit;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	TOptional<FGridAddress> PreHighlightGridAddress;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Selection")
	TObjectPtr<UNGItemInstance> CurrentDraggingItem;
	
	// UPROPERTY()
	// TObjectPtr<ANGHUD> NGHUD;
	
/*************************************/
/*				리롤 관련			 */
/*************************************/
public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Shop")
	void Server_RequestSellUnit(ANGUnitPawn* NewPawn);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Shop")
	void Server_RequestBuyUnit(FGameplayTag UnitTag);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Map")
	void Server_SelectNode(int32 NodeID);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Map")
	void Server_RollMovementDice();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Map")
	void Server_CompleteNodeAction();

	UFUNCTION(Client, Reliable)
	void Client_BeginNodeAction(ENodeType NodeType, int32 NodeID);

	UFUNCTION(Client, Reliable)
	void Client_BeginPvPCombat(ANGPlayerState* OpponentPlayer, int32 NodeID);

	UFUNCTION(Client, Reliable)
	void Client_ShowCombatResult(const FCombatResultData& CombatResult);
	
	UFUNCTION(Client, Reliable)
	void Client_OnBuyUnit(bool bIsSuccess);
	
	UNGPocketComponent* GetPlayerPocket() const;
	void EnterPhase(EGamePhase Phase);

	UPROPERTY(BlueprintAssignable, Category = "Game|Inventory")
	FOnShowInventory OnShowInventory;
	
	UPROPERTY(BlueprintAssignable, Category = "Game|Shop")
	FOnBuyUnitSuccessSignature OnBuyUnitSuccess;

	UPROPERTY(BlueprintAssignable, Category = "Game|Map")
	FOnNodeActionStartedSignature OnNodeActionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Game|Combat")
	FOnPvPCombatStartedSignature OnPVPCombatStarted;

	UPROPERTY(BlueprintAssignable, Category = "Game|Combat")
	FOnCombatResultSignature OnCombatResult;
	
/*************************************/
/*				전투					 */
/*************************************/
public:
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Combat")
	void Server_EnterPhase(EGamePhase Phase);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game|Combat")
	void Server_RequestFlee();
	
/*************************************/
/*				UI					 */
/*************************************/

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNGUnitInfoWidget> UnitInfoWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UNGUnitInfoWidget> UnitInfoWidgetInstance;

/*************************************/
/*				Inventory			 */
/*************************************/

public:
	void AddItem(const FGameplayTag& ItemTag) const;

protected:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = "true"))
	int32 ProcessPendingItemMaxPerFrame;
	
	UPROPERTY()
	TArray<FGameplayTag> PendingItemTags;
	
/*************************************/
/*				Debug				 */
/*************************************/

public:


	UFUNCTION(Server,Reliable)
	void Server_RequestToggleJohnAppeared();
	
	UFUNCTION(Server, Reliable)
	void Server_RequestStartCombat(bool bIsCPUCombat);
	
	UFUNCTION(Server, Reliable)
	void Server_RequestStopCombat();

	UFUNCTION(Server, Reliable)
	void Server_RequestGetItem(const FString& ItemName, int32 Count);
	
	UFUNCTION(Exec)
	void Cmd_StartCombat(bool bIsCPUCombat);

	UFUNCTION(Exec)
	void Cmd_FinishCombat();
	
	UFUNCTION(Exec)
	void Cmd_ToggleDebugGrid();

	UFUNCTION(Exec)
	void Cmd_ToggleJohn();
	
	UFUNCTION(Exec)
	void Cmd_GetItem(const FString& ItemName, int32 Count);
	
private:
	bool bShowDebugGrid;
	
	
/*************************************/
/*					일반				 */
/*************************************/
	
	
};
