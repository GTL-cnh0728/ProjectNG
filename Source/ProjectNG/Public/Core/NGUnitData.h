// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "NGUnitData.generated.h"

class ANGUnitPawn;
class UNGPawnAnimationSet;

UENUM(BlueprintType)
enum class EUnitTier : uint8
{
	Tier1,
	Tier2,
	Tier3
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FUnitData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", DisplayName = "유닛 식별 태그")
	FGameplayTag IdentificationTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", DisplayName = "UI 표시 이름")
	FText UnitName = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", DisplayName = "유닛 코스트")
	FGameplayTag CostTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", DisplayName = "기본 등급")
	EUnitTier Tier = EUnitTier::Tier1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Identity", DisplayName = "베이스 유닛 이름", meta=(ToolTip="등급이 달라도 같은 유닛인지 묶는 기준입니다. 예: Warrior_T1/T2/T3 모두 Warrior"))
	FName BaseUnitName = FName();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Merge", DisplayName = "다음 등급 태그", meta=(EditCondition = "Tier != EUnitTier::Tier3"))
	FGameplayTag NextTierTag = FGameplayTag();

	/** 이전 등급 태그 (자동 역매핑 — NGUnitDataManager::Initialize()에서 설정) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Merge", DisplayName = "이전 등급 태그")
	FGameplayTag PrevTierTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Merge", DisplayName = "합성 필요 개수")
	int32 MergeRequiredCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", DisplayName = "생성할 유닛 폰 클래스")
	TSubclassOf<ANGUnitPawn> UnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", DisplayName = "유닛 이미지")
	TSoftObjectPtr<UTexture2D> UnitTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool", DisplayName = "유닛 총 개수", meta=(ClampMin = "-1", ToolTip="-1 이면 제한 없음으로 사용할 수 있습니다."))
	int32 TotalCountInPool = -1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Price")
	float Price = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", DisplayName = "유닛이 사용할 애니메이션 셋")
	TObjectPtr<UNGPawnAnimationSet> AnimationSet;
	
};
