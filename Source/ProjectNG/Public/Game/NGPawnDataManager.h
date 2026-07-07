// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/EnemyAbilityData.h"
#include "Core/NGUnitData.h"
#include "Core/UnitAbilityDataRow.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NGPawnDataManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGPawnDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	const FUnitAbilityData* GetUnitAbilityData(const FGameplayTag IdentificationTag);
	
	const FUnitData* GetUnitData(const FGameplayTag IdentificationTag);
	
	FName GetUnitName(const FGameplayTag IdentificationTag) const;

	/** 해당 태그의 1성 기본 유닛 태그를 반환합니다 (재귀 역추적) */
	FGameplayTag GetBaseTierTag(const FGameplayTag IdentificationTag) const;

	/** 해당 유닛을 판매할 때 공용 풀로 반환해야 할 1성 유닛 수를 계산합니다 */
	int32 GetDecomposedBaseUnitCount(const FGameplayTag IdentificationTag) const;
	float GetUnitPrice(FGameplayTag IdentificationTag) const;

	bool IsExistUnitDataTable() const { return UnitDataTable != nullptr; }
	
	const UDataTable* GetUnitDataTable() { return UnitDataTable; }
	
	const UDataTable* GetUnitAbilityDataTable() { return UnitAbilityDataTable; }
	
	const TMap<FGameplayTag, FUnitData*>& GetAllUnitDataMap() const { return TagToUnitDataMap; }
	
private:
	UPROPERTY()
	TObjectPtr<UDataTable> UnitAbilityDataTable;
	
	UPROPERTY()
	TObjectPtr<UDataTable> UnitDataTable;
	
	TMap<FGameplayTag, FUnitAbilityData*> TagToUnitAbilityDataMap;
	
	TMap<FGameplayTag, FUnitData*> TagToUnitDataMap;
	
	TMap<FGameplayTag, FName> TagToUnitNameDataMap;
};
