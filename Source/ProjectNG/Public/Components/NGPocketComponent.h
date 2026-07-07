// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NGPocketComponent.generated.h"


enum class EUnitTier : uint8;
class ANGUnitPawn;
class ANGPawnBase;

UENUM(BlueprintType)
enum class EShopActionType : uint8
{
	None,
	Roll,
	Buy,
	Sell,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGPocketComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNGPocketComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override; // Server

	UFUNCTION(BlueprintCallable, Category = "Game|Shop")
	void RequestRoll();

	UFUNCTION(BlueprintPure, Category = "Game|Shop")
	const TArray<FGameplayTag>& GetRollPocket() const { return RollShopPocket; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Shop")
	int32 PlayerLevel = 1; // TODO. 어빌리티 시스템으로 처리해야함. 현재 디버깅용으로 임시 변수

	void RemoveUnitFromShop(FGameplayTag UnitTag);
	void TryMergeUnit(FGameplayTag IdentificationTag);

	void SellUnit(ANGPawnBase* UnitToSell);

protected:
	UFUNCTION(Server, Reliable)
	void Server_RequestRoll();
	
	UFUNCTION()
	void OnRep_RollPocket();
	
	UFUNCTION()
	void UpdateRollUnit();
	
	void CheckAndMergeUnit(FGameplayTag IdentificationTag);

	/**
	 * 유닛 태그를 재귀적으로 분해하여 1성 유닛의 Tag 목록을 반환합니다.
	 * 2성 → 1성 Tag × 3, 3성 → 1성 Tag × 9
	 */
	void DecomposeToBaseUnits(const FGameplayTag& UnitTag, TArray<FGameplayTag>& OutBaseUnitTags) const;
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayMergeEffect(FVector EffectLocation, EUnitTier UnitTier);

	UPROPERTY(EditDefaultsOnly, Category = "Game|Shop")
	int32 ShopSlotCount = 3;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_RollPocket, Category = "Game|Shop")
	TArray<FGameplayTag> RollShopPocket; // 플레이어의 상점에 표시된 유닛 목록
	
	UPROPERTY(Replicated, VisibleAnywhere)
	EShopActionType LastShopAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game|Shop")
	TObjectPtr<UDataTable> ProbabilityTable;

public:
	bool bDebugJohnAppeared = true;
	
	void GetPlacedUnits(TArray<ANGPawnBase*>& OutUnits);
	
	bool IsAnnihilated();
	
	void ControlPocketSpawning(ANGPawnBase* NewPawn);
	void ControlPocketSelling(ANGPawnBase* NewPawn);
	void ReturnUnitToShop(ANGPawnBase* NewPawn);
	void AddUnitFromPocket(ANGPawnBase* NewPawn);

	void RemoveUnitFromPocket(ANGPawnBase* Unit);
	
	TArray<ANGPawnBase*>& GetOwnedUnitPocket()	{ return OwnedUnitPocket; }

	void CollectTotalUnitHPAndMaxHP(float& OutMaxHP, float& OutHP);
	
	void SetTotalUnitHPSnapShot(float InTotalHP){TotalUnitHPSnapShot = InTotalHP;}
	float GetTotalUnitHPSnapShot() const {return TotalUnitHPSnapShot;}
	
	void SetTotalUnitMaxHPSnapShot(float InMaxTotalHP){TotalUnitMaxHPSnapShot = InMaxTotalHP;}
	float GetTotalUnitMaxHPSnapShot() const {return TotalUnitMaxHPSnapShot;}
	
private:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Game|Unit")
	TArray<ANGPawnBase*> OwnedUnitPocket;
	
	UPROPERTY()
	float TotalUnitHPSnapShot;

	UPROPERTY()
	float TotalUnitMaxHPSnapShot;
	
/*************************************/
/*				Debug용				 */
/*************************************/
public:
	UFUNCTION(BlueprintCallable, Category = "Debug")
	FString GetDebugNetMode() const
	{
		ENetMode NetMode = GetNetMode();
		switch (NetMode)
		{
		case NM_Client: return TEXT("Client");
		case NM_ListenServer: return TEXT("ListenServer");
		case NM_DedicatedServer: return TEXT("DedicatedServer");
		default: return TEXT("Standalone");
		}
	}
	
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void Debug_CheckBinding(UObject* WidgetRef) {
		UE_LOG(LogTemp, Warning, TEXT("[Bind] Pocket Addr: %p, Owner Addr: %p"), this, WidgetRef);
	}
};
