// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/NGAbilitySystemComponent.h"
#include "Combat/Grid/Grid.h"
#include "Components/NGCombatManagerComponent.h"
#include "GameFramework/PlayerState.h"
#include "NGPlayerState.generated.h"

class UNGBuffManagerComponent;
class UNGInventoryComponent;
class UNGPlayerAttributeSet;
class AArenaManager;
class AArena;
class UNGPocketComponent;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None,
	Maintaining,
	Combat,
	Exploration,
	GameOver
};

/**
 * In-Game에서의 플레이어의 상태 정보를 저장하는 클래스
 *
 * 현재 보유 골드, 레벨, 경험치, 남은 체력 등 게임 판 내의 플레이어 정보를 저장하고 모든 클라이언트에게 복제(Replicate)할 때 사용
 * 플레이어가 구매하여 보유 중인 **유닛들의 목록(로스터)**을 관리
 */
UCLASS()
class PROJECTNG_API ANGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ANGPlayerState();
	
	//~Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	//~End IAbilitySystemInterface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	UNGAbilitySystemComponent* GetNGAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	void InitializePostLogin();

	void SpawnGridMapManager();
	
	void CaptureSnapShot();
	void OnEnterGameState(const EGameState& NewState);
	void OnExitGameState(const EGameState& PreState);

protected:
	UPROPERTY()
	TObjectPtr<UNGAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UNGPlayerAttributeSet> AttributeSet;

/*************************************/
/*			WorldMap Phase 관련		 */
/*************************************/
	
public:
	const FGameplayTag& GetCurrentZoneTag() const { return CurrentZoneTag; }

	void SetGameState(EGameState NewState);
	void EarnGold(float EarnedGold) const;
	
	UFUNCTION(BlueprintCallable)
	EGameState GetGameState() const { return CurrentGameState; }

	int32 GetCurrentNodeID() const { return CurrentNodeID; }
	void SetCurrentNodeID(int32 NewNodeID) { CurrentNodeID = NewNodeID; }

	int32 GetTargetNodeID() const { return TargetNodeID; }
	void SetTargetNodeID(int32 NewNodeID) { TargetNodeID = NewNodeID; }

	bool HasSelectedNode() const { return bHasSelectedNode; }
	void SetHasSelectedNode(bool bSelected) { bHasSelectedNode = bSelected; }

	bool IsActionFinished() const { return bIsActionFinished; }
	void SetActionFinished(bool bFinished) { bIsActionFinished = bFinished; }

	bool ShouldSkipNextMovementTurn() const { return bSkipNextMovementTurn; }
	void SetSkipNextMovementTurn(bool bSkip) { bSkipNextMovementTurn = bSkip; }
	bool DidSkipMovementThisTurn() const { return bSkippedMovementThisTurn; }
	void SetSkippedMovementThisTurn(bool bSkipped) { bSkippedMovementThisTurn = bSkipped; }

	void OnCombatEnd(FCombatResultData CombatResult);
	bool ConsumePendingCombatResult(FCombatResultData& OutCombatResult);
	
	float GetOwnedGold() const;
	
protected:
	void OnCombatWin();
	void OnCombatLose();
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	EGameState CurrentGameState;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FGameplayTag CurrentZoneTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|AbilitySystemComponent")
	TObjectPtr<UDataTable> DefaultPlayerAttributeTable;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game|Turn")
	int32 CurrentNodeID = -1;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game|Turn")
	int32 TargetNodeID = -1;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game|Turn")
	bool bHasSelectedNode = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Game|Turn")
	bool bIsActionFinished = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game|Turn")
	bool bSkipNextMovementTurn = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game|Turn")
	bool bSkippedMovementThisTurn = false;

	FCombatResultData PendingCombatResult;
	bool bHasPendingCombatResult = false;

/*************************************/
/*		전투 및 Pocket 관련			 */
/*************************************/
	
public:
	FORCEINLINE UNGPocketComponent* GetPlayerPocket() const { return PlayerPocket; }
	FORCEINLINE UNGInventoryComponent* GetPlayerInventory() const { return PlayerInventory; }
	FORCEINLINE UNGBuffManagerComponent* GetBuffManager() const { return PlayerBuffManager; }

	int32 GetUserIndex();
	
	void AddCPUEnemyCount();
	
	void OnDieCPUEnemy() { ++CPUEnemyDieCount; }
	
	bool IsCPUCombatFinished() const
	{
		UE_LOG(LogTemp, Log, TEXT("CurrentCPUEnemyCount %d, CPUEnemyDieCount %d"), CurrentCPUEnemyCount, CPUEnemyDieCount);
		return CurrentCPUEnemyCount <= CPUEnemyDieCount;
	}
	
	void InitCPUCombat(const FEnemySquadData& SquadData);
	
protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	TObjectPtr<UNGPocketComponent> PlayerPocket;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	TObjectPtr<UNGInventoryComponent> PlayerInventory;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	TObjectPtr<UNGBuffManagerComponent> PlayerBuffManager;
	
	UPROPERTY()
	int32 CurrentCPUEnemyCount;

	UPROPERTY()
	int32 CPUEnemyDieCount;
	
	/*************************************/
	/*				GridMap 관련			 */
	/*************************************/
public:
	FHexGridMap& GetCombatGridMap() { return CombatGridMap; }
	FQuadGridMap& GetEnemyWaitGridMap() { return EnemyWaitGridMap; }
	FQuadGridMap& GetWaitGridMap() { return WaitGridMap; }
	
	AArena* GetHomeArena() const { return HomeArena; }
	AArenaManager* GetArenaManager() const { return ArenaManager; }
	
	UFUNCTION()
	void RestoreInitialGrid();
	
	void StartCombat();
	void FinishCombat();

protected:
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	FHexGridMap CombatGridMap;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	FQuadGridMap WaitGridMap;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category = "Grid")
	FQuadGridMap EnemyWaitGridMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grid")
	FHexGridMap CombatGridMapSnapShot;
	
	//그리드 맵의 실체를 담당(각 유저당 하나임)
	UPROPERTY(Replicated)
	TObjectPtr<AArena> HomeArena;	
	
	UPROPERTY()
	TObjectPtr<AArenaManager> ArenaManager;

};
