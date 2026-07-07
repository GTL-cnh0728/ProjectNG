// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Components/NGPocketComponent.h"

#include "AbilitySystem/NGPawnAttributeSet.h"
#include "AbilitySystem/NGPlayerAttributeSet.h"
#include "Components/NGBuffManagerComponent.h"
#include "Core/NGPoolSubSystem.h"
#include "Core/NGShopProbability.h"
#include "Core/NGSpawnHelper.h"
#include "Core/NGUnitData.h"
#include "Game/NGPawnDataManager.h"
#include "GameModes/NGInGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Pawn/NGPawnBase.h"
#include "Player/NGPlayerController.h"


UNGPocketComponent::UNGPocketComponent()
{
	SetIsReplicatedByDefault(true);
}

void UNGPocketComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UNGPocketComponent, RollShopPocket, COND_OwnerOnly); //요청보낸 주인한테만 동기화
	DOREPLIFETIME_CONDITION(UNGPocketComponent, LastShopAction, COND_OwnerOnly); 
	DOREPLIFETIME(UNGPocketComponent, OwnedUnitPocket);
}

void UNGPocketComponent::RequestRoll()
{
	AActor* MyOwner = GetOwner();
	FString RoleStr = MyOwner ? MyOwner->HasAuthority() ? TEXT("[SERVER]") : TEXT("[CLIENT]") : TEXT("");
    
	bool bHasConnection = MyOwner && (MyOwner->GetNetConnection() != nullptr);
	// 1. 내 주인(PC)이 서버와 연결된 소켓(NetConnection)을 가지고 있는지 확인!
    
	UE_LOG(LogTemp, Warning, TEXT("%s [RequestRoll] Addr: %p | Has NetConnection: %s"), 
		*RoleStr, this, bHasConnection ? TEXT("TRUE") : TEXT("FALSE"));

	if (!bHasConnection && !MyOwner->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("!!! ERROR !!! 당신은 지금 네트워크 연결이 없는 가짜 객체에서 RPC를 시도했습니다. 패킷이 증발합니다!"));
		return;
	}

	Server_RequestRoll();
}

void UNGPocketComponent::TryMergeUnit(FGameplayTag IdentificationTag)
{
	CheckAndMergeUnit(IdentificationTag);
}

void UNGPocketComponent::OnRep_RollPocket()
{
	switch (LastShopAction)
	{
		case EShopActionType::Buy:
		
			break;
		case EShopActionType::Roll:
			UpdateRollUnit();
			break;
		default:
			break;
	}
}

void UNGPocketComponent::UpdateRollUnit()
{
	if (ANGPlayerState* PS = GetOwner<ANGPlayerState>())
	{
		if (ANGPlayerController* PC = Cast<ANGPlayerController>(PS->GetPlayerController()))
		{
			FString RoleStr = GetOwner()->HasAuthority() ? TEXT("[SERVER]") : TEXT("[CLIENT]");
			auto Owner = GetOwner();
			
			UE_LOG(LogTemp, Warning, TEXT("[OnRep] Pocket Addr: %p, Owner Addr: %p"), this, Owner);
			
			// 델리게이트 바인딩 상태 확인
			if (PC->OnUnitsUpdated.IsBound())
			{
				UE_LOG(LogTemp, Warning, TEXT("OnRep_RollChange: Delegate is Bound. Broadcaster Addr: %p"), &(PC->OnUnitsUpdated));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("OnRep_RollChange: NO ONE is listening to this delegate!"));
			}
			PC->OnUnitsUpdated.Broadcast();
		}
	}
}

void UNGPocketComponent::CheckAndMergeUnit(FGameplayTag IdentificationTag)
{
	// 서버가 아니면 리턴
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	UNGPawnDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UNGPawnDataManager>();
	if (!DataManager) return;
	
	TArray<ANGPawnBase*> SameUnits;
	for (ANGPawnBase* Unit : OwnedUnitPocket)
	{
		if (Unit && Unit->GetIdentificationTag() == IdentificationTag)
		{
			SameUnits.Add(Unit);
		}
	}
	
	if (SameUnits.Num() >= 3)
	{
		const FUnitData* UnitData = DataManager->GetUnitData(IdentificationTag);
		if (!UnitData || !UnitData->NextTierTag.IsValid()) return;

		const int32 MergeRequiredCount = FMath::Max(UnitData->MergeRequiredCount, 3);
		if (SameUnits.Num() < MergeRequiredCount) return;
		
		TArray<ANGPawnBase*> Materials;
		Materials.Reserve(MergeRequiredCount);
		for (int32 i = 0; i < MergeRequiredCount; ++i)
		{
			Materials.Add(SameUnits[i]);
		}

		FGridAddress MergeGridAddress = Materials.Last()->GetGridAddress();
		for (ANGPawnBase* Material : Materials)
		{
			if (Material && Material->GetGridAddress().GridType == EGridType::Combat)
			{
				MergeGridAddress = Material->GetGridAddress();
				break;
			}
		}
		
		FVector MergeLocation = UGridMapHelper::GetWorldLocation(MergeGridAddress);

		UNGPoolSubSystem* Pool = GetWorld()->GetSubsystem<UNGPoolSubSystem>();
		if (!Pool) return;
		
		for (ANGPawnBase* Material : Materials)
		{
			if (!Material) continue;
			
			Material->UnSetPawnOnGrid(Material->GetGridAddress());
			RemoveUnitFromPocket(Material);  // OwnedPocket에서만 제거 (상점 풀 미반환)
			Pool->ReleaseSegment(Material);
		}
		
		// 상위 등급 생성
		if (ANGPlayerState* PS = GetOwner<ANGPlayerState>())
		{
			if (ANGPlayerController* PC = Cast<ANGPlayerController>(PS->GetPlayerController()))
			{
				// 이 스폰이 성공하면 다시 ControlPocketSpawning이 호출되며 연쇄 작용 일어남
				if (UNGSpawnHelper::SpawnUnitPawnAtGrid(PC, UnitData->NextTierTag, MergeGridAddress))
				{
					// 클라이언트 RPC 연출 호출
					if (const FUnitData* NextData = DataManager->GetUnitData(UnitData->NextTierTag))
					{
						Multicast_PlayMergeEffect(MergeLocation, NextData->Tier);
					}
				}
			}
		}
	}
}

void UNGPocketComponent::SellUnit(ANGPawnBase* UnitToSell)
{
	if (!UnitToSell) return;

	// 내 소유 유닛인지 검증
	if (!OwnedUnitPocket.Contains(UnitToSell)) return;

	UNGPawnDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UNGPawnDataManager>();
	UNGPoolSubSystem* Pool = GetWorld()->GetSubsystem<UNGPoolSubSystem>();
	if (!DataManager || !Pool) return;
	
	// 판매 대상 유닛을 소유 목록에서 제거 + 오브젝트 풀로 반환
	UnitToSell->UnSetPawnOnGrid(UnitToSell->GetGridAddress());
	ControlPocketSelling(UnitToSell);
	Pool->ReleaseSegment(UnitToSell);
}

void UNGPocketComponent::DecomposeToBaseUnits(const FGameplayTag& UnitTag, TArray<FGameplayTag>& OutBaseUnitTags) const
{
	UNGPawnDataManager* DataManager = GetWorld()->GetGameInstance()->GetSubsystem<UNGPawnDataManager>();
	if (!DataManager) return;

	const FUnitData* UnitData = DataManager->GetUnitData(UnitTag);
	if (!UnitData) return;

	// 1성이면 자기 자신의 태그를 추가하고 종료 (재귀 종료 조건)
	if (UnitData->Tier == EUnitTier::Tier1 || !UnitData->PrevTierTag.IsValid())
	{
		OutBaseUnitTags.Add(UnitTag);
		return;
	}

	// 2성 이상이면, MergeRequiredCount만큼 하위 티어를 재귀 분해
	const int32 Count = FMath::Max(UnitData->MergeRequiredCount, 3);
	for (int32 i = 0; i < Count; ++i)
	{
		DecomposeToBaseUnits(UnitData->PrevTierTag, OutBaseUnitTags);
	}
}

void UNGPocketComponent::Multicast_PlayMergeEffect_Implementation(FVector EffectLocation, EUnitTier UnitTier)
{
	// 이 함수는 서버에서 호출되더라도 모든 클라이언트(플레이어의 화면)에서 실행됩니다.
	// 이 안에서 무거운 이펙트 생성, 파티클 폭발, 티어별 사운드 재생 등을 수행합니다.
}

void UNGPocketComponent::GetPlacedUnits(TArray<ANGPawnBase*>& OutUnits)
{
	for (ANGPawnBase* Unit : OwnedUnitPocket)
	{
		if (Unit)
		{
			if (Unit->GetGridAddress().GridType == EGridType::Combat)
			{
				OutUnits.Add(Unit);
			}
		}
	}
}

bool UNGPocketComponent::IsAnnihilated()
{
	TArray<ANGPawnBase*> PlacedUnitPocket;
	GetPlacedUnits(PlacedUnitPocket);
	
	for (ANGPawnBase* Unit : PlacedUnitPocket)
	{
		if (!Unit->IsDead())
		{
			return false;
		}
	}
	
	return true;
}

void UNGPocketComponent::ControlPocketSpawning(ANGPawnBase* NewPawn)
{
	RemoveUnitFromShop(NewPawn->GetIdentificationTag());
	AddUnitFromPocket(NewPawn);
	
	ANGPlayerState* PS = GetOwner<ANGPlayerState>();
	if (UNGBuffManagerComponent* BuffManager = PS->GetBuffManager())
	{
		BuffManager->ApplyAllBuffsToPawn(NewPawn);
	}
}

void UNGPocketComponent::ControlPocketSelling(ANGPawnBase* NewPawn)
{
	ReturnUnitToShop(NewPawn);
	RemoveUnitFromPocket(NewPawn);
}

void UNGPocketComponent::RemoveUnitFromShop(FGameplayTag UnitTag)
{
	LastShopAction = EShopActionType::Buy;
	RollShopPocket.Remove(UnitTag);
}

void UNGPocketComponent::ReturnUnitToShop(ANGPawnBase* NewPawn)
{
	if (!NewPawn) return;

	UNGAbilitySystemComponent* ASC = NewPawn->GetNGAbilitySystemComponent();
	const UNGPawnAttributeSet* AttributeSet = ASC->GetSet<UNGPawnAttributeSet>();
	ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
	if (!GM || !ASC || !AttributeSet) return;
	
	LastShopAction = EShopActionType::Sell;

	const FGameplayTag UnitTag = NewPawn->GetIdentificationTag();

	// 1. 재귀적으로 1성 유닛 목록 분해
	TArray<FGameplayTag> BaseUnitTags;
	DecomposeToBaseUnits(UnitTag, BaseUnitTags);

	UE_LOG(LogTemp, Log, TEXT("[판매] %s 판매 → 1성 유닛 %d개 공용 풀 반환"), *UnitTag.ToString(), BaseUnitTags.Num());

	// 2. 분해된 1성 유닛들을 상점 공용 풀로 반환
	for (const FGameplayTag& BaseUnitTag : BaseUnitTags)
	{
		GM->ReturnUnitToPool(BaseUnitTag, 1);
	}
}

void UNGPocketComponent::AddUnitFromPocket(ANGPawnBase* NewPawn)
{
	if (!NewPawn)	return;
	
	OwnedUnitPocket.AddUnique(NewPawn);
	TryMergeUnit(NewPawn->GetIdentificationTag());
}

void UNGPocketComponent::RemoveUnitFromPocket(ANGPawnBase* Unit)
{
	OwnedUnitPocket.Remove(Unit);
}

void UNGPocketComponent::CollectTotalUnitHPAndMaxHP(float& OutMaxHP, float& OutHP)
{
	float TotalHP = 0;
	float TotalMaxHP = 0;
	TArray<ANGPawnBase*> PlacedUnitPocket;
	GetPlacedUnits(PlacedUnitPocket);
	for (ANGPawnBase* Unit : PlacedUnitPocket)
	{
		UNGAbilitySystemComponent* ASC = Unit->GetNGAbilitySystemComponent();
		const UNGPawnAttributeSet* PawnAttributeSet = ASC ? ASC->GetSet<UNGPawnAttributeSet>() : nullptr;
		TotalHP += PawnAttributeSet ? PawnAttributeSet->GetHealth() : 0.f;
		TotalMaxHP += PawnAttributeSet ? PawnAttributeSet->GetMaxHealth() : 0.f;
	}
	
	OutMaxHP = TotalMaxHP;
	OutHP = TotalHP;
}

void UNGPocketComponent::Server_RequestRoll_Implementation()
{
	if (!GetOwner()->HasAuthority())	return;
	
	checkf(ProbabilityTable, TEXT("[PocketComponent] Not initialized Probability table."))
	
	if (!ProbabilityTable) return;

	ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
	if (!GM) return;
	
	ANGPlayerState* PS = GetOwner<ANGPlayerState>();
	UNGAbilitySystemComponent* ASC = PS ? PS->GetNGAbilitySystemComponent() : nullptr;
	const UNGPlayerAttributeSet* AttributeSet = ASC ? ASC->GetSet<UNGPlayerAttributeSet>() : nullptr;
	
	// 레벨에 맞는 확률 데이터 가져오기
	FString RowName = FString::FromInt(AttributeSet ? AttributeSet->GetLevel() : 1);
	FShopProbability* ProbabilityData = ProbabilityTable->FindRow<FShopProbability>(*RowName, TEXT(""));
	
	if (!ProbabilityData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProbabilityData is not found, PlayerLevel has invalid value."));
		return;
	}
	
	// 기존 포켓 안의 유닛을 다시 반환
	for (FGameplayTag UnitToReturn : RollShopPocket)
	{
		GM->ReturnUnitToPool(UnitToReturn, 1);
	}
	RollShopPocket.Empty();
	
	for (int32 i = 0; i < ShopSlotCount; ++i)
	{
		float RollValue = FMath::FRand(); // 0.0 - 1.0 사이의 랜덤 값

		EUnitTier SelectedTier;
		if (RollValue < ProbabilityData->PercentOfTier1)
		{
			SelectedTier = EUnitTier::Tier1;
		}
		else if (RollValue < ProbabilityData->PercentOfTier1 + ProbabilityData->PercentOfTier2)
		{
			SelectedTier = EUnitTier::Tier2;
		}
		else
		{
			SelectedTier = EUnitTier::Tier3;
		}

		FGameplayTag SelectedUnitTag = GM->GetRandomUnitByTier(SelectedTier);
		if (GM->IsExistUnitDataTable())
		{
			if (SelectedUnitTag.IsValid() && GM->IsExistUnit(SelectedUnitTag))
			{
				GM->GrabUnitFromPool(SelectedUnitTag);
				RollShopPocket.Add(SelectedUnitTag);
			}
			else
			{
				
				i--; // 다시 시도
			}
		}else
		{
			UE_LOG(LogTemp, Error, TEXT("DataTabel isn't Exist!"));
		}
	}

	//디버깅용도
	if (bDebugJohnAppeared)
	{
		RollShopPocket[0] = FGameplayTag::RequestGameplayTag(TEXT("Unit.Tier1.John"));
	}
	
	LastShopAction = EShopActionType::Roll;
	
	// 서버에서는 롤포켓할필요 없지않나
	// OnRep_RollPocket();
}
