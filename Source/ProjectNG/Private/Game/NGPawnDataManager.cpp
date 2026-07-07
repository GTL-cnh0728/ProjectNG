// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Game/NGPawnDataManager.h"

void UNGPawnDataManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FString AbilityDataTablePath = TEXT("/Game/DataTables/DT_UnitAbilityData.DT_UnitAbilityData"); 
	UnitAbilityDataTable = LoadObject<UDataTable>(nullptr, *AbilityDataTablePath);

	const FString UnitDataTablePath = TEXT("/Game/DataTables/DT_UnitData_kr.DT_UnitData_kr"); 
	UnitDataTable = LoadObject<UDataTable>(nullptr, *UnitDataTablePath);
	
	if (UnitAbilityDataTable)
	{
		TArray<FUnitAbilityData*> AllRows;
		UnitAbilityDataTable->GetAllRows<FUnitAbilityData>(TEXT("InitializeUnitMap"), AllRows);

		for (FUnitAbilityData* RowData : AllRows)
		{
			if (RowData && RowData->IdentificationTag.IsValid())
			{
				TagToUnitAbilityDataMap.Add(RowData->IdentificationTag, RowData);
			}
		}
        
		UE_LOG(LogTemp, Log, TEXT("유닛 능력치 데이터 테이블 캐싱 완료! 총 %d개"), TagToUnitAbilityDataMap.Num());
	}
	
	if (UnitDataTable)
	{
		for (auto& Pair : UnitDataTable->GetRowMap())
		{
			FName RowName = Pair.Key;
			FUnitData* RowData = (FUnitData*)Pair.Value;
		
			if (RowData && RowData->IdentificationTag.IsValid())
			{
				TagToUnitDataMap.Add(RowData->IdentificationTag, RowData);
				TagToUnitNameDataMap.Add(RowData->IdentificationTag, RowName);
			}
		}

		// NextTierTag를 기반으로 PrevTierTag 자동 역매핑
		for (auto& [Tag, Data] : TagToUnitDataMap)
		{
			if (Data && Data->NextTierTag.IsValid())
			{
				if (FUnitData** NextData = TagToUnitDataMap.Find(Data->NextTierTag))
				{
					(*NextData)->PrevTierTag = Tag;
				}
			}
		}
		
		UE_LOG(LogTemp, Log, TEXT("유닛 데이터 테이블 캐싱 완료! 총 %d개 (PrevTierTag 역매핑 완료)"), TagToUnitDataMap.Num());
	}
}

const FUnitAbilityData* UNGPawnDataManager::GetUnitAbilityData(const FGameplayTag IdentificationTag)
{
	if (!IdentificationTag.IsValid()) return nullptr;
	
	if (FUnitAbilityData* const* FoundData = TagToUnitAbilityDataMap.Find(IdentificationTag))
	{
		return *FoundData; // 포인터의 포인터이므로 한 번 역참조해서 반환
	}
	
	UE_LOG(LogTemp, Warning, TEXT("해당 태그(%s)를 가진 유닛 데이터가 없습니다!"), *IdentificationTag.ToString());
	return nullptr;
}

const FUnitData* UNGPawnDataManager::GetUnitData(const FGameplayTag IdentificationTag)
{
	if (!IdentificationTag.IsValid()) return nullptr;
	
	if (FUnitData* const* FoundData = TagToUnitDataMap.Find(IdentificationTag))
	{
		return *FoundData; // 포인터의 포인터이므로 한 번 역참조해서 반환
	}
	
	UE_LOG(LogTemp, Warning, TEXT("해당 태그(%s)를 가진 유닛 데이터가 없습니다!"), *IdentificationTag.ToString());
	return nullptr;
}

FName UNGPawnDataManager::GetUnitName(const FGameplayTag IdentificationTag) const
{
	if (const FName* FoundRowName = TagToUnitNameDataMap.Find(IdentificationTag))
	{
		return *FoundRowName;
	}
	return NAME_None;
}

FGameplayTag UNGPawnDataManager::GetBaseTierTag(const FGameplayTag IdentificationTag) const
{
	if (!IdentificationTag.IsValid()) return FGameplayTag();

	FUnitData* const* FoundData = TagToUnitDataMap.Find(IdentificationTag);
	if (!FoundData || !(*FoundData)) return FGameplayTag();

	// T1이거나 PrevTierTag가 없으면 자기 자신이 기본 유닛
	if ((*FoundData)->Tier == EUnitTier::Tier1 || !(*FoundData)->PrevTierTag.IsValid())
	{
		return IdentificationTag;
	}

	// 재귀적으로 하위 티어 추적
	return GetBaseTierTag((*FoundData)->PrevTierTag);
}

int32 UNGPawnDataManager::GetDecomposedBaseUnitCount(const FGameplayTag IdentificationTag) const
{
	if (!IdentificationTag.IsValid()) return 0;

	FUnitData* const* FoundData = TagToUnitDataMap.Find(IdentificationTag);
	if (!FoundData || !(*FoundData)) return 0;

	// T1이면 1개
	if ((*FoundData)->Tier == EUnitTier::Tier1 || !(*FoundData)->PrevTierTag.IsValid())
	{
		return 1;
	}

	// 하위 티어의 분해 수 × 합성 필요 개수
	const int32 MergeCount = FMath::Max((*FoundData)->MergeRequiredCount, 3);
	return MergeCount * GetDecomposedBaseUnitCount((*FoundData)->PrevTierTag);
}

float UNGPawnDataManager::GetUnitPrice(const FGameplayTag IdentificationTag) const
{
	if (!IdentificationTag.IsValid()) return 0;

	FUnitData* const* FoundData = TagToUnitDataMap.Find(IdentificationTag);
	if (!FoundData || !(*FoundData)) return 0;

	return (*FoundData)->Price;
}