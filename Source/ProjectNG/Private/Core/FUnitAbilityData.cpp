#include "Core/UnitAbilityDataRow.h"
#include "AbilitySystem/NGPawnAttributeSet.h"

FUnitAbilityData::FUnitAbilityData()
{
	// =========================================================================
    // Vital Attributes (생존/자원 관련 디폴트 세팅)
    // =========================================================================
    DefaultStats.Add(UNGPawnAttributeSet::GetMaxHealthAttribute(), 100.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetHealthAttribute(), 100.f);
    
    DefaultStats.Add(UNGPawnAttributeSet::GetMaxManaAttribute(), 100.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetManaAttribute(), 50.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetManaRegenerationAttribute(), 10.f);

    // =========================================================================
    // Stats & Primary Attributes (기본 주능력치)
    // =========================================================================
    DefaultStats.Add(UNGPawnAttributeSet::GetAgilityAttribute(), 10.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetAttackDamageAttribute(), 30.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetAbilityPowerAttribute(), 30.f);

    // =========================================================================
    // Combat Attributes (전투/계산 관련)
    // =========================================================================
    DefaultStats.Add(UNGPawnAttributeSet::GetAttackRangeAttribute(), 1.f); 
    DefaultStats.Add(UNGPawnAttributeSet::GetAttackSpeedAttribute(), 1.f);  // 초당 1회 공격 기준
    DefaultStats.Add(UNGPawnAttributeSet::GetCriticalRateAttribute(), 0.f);  // 치명타율 (0.0 ~ 1.0)
    DefaultStats.Add(UNGPawnAttributeSet::GetDodgeRateAttribute(), 0.f);     // 회피율 (0.0 ~ 1.0)
    DefaultStats.Add(UNGPawnAttributeSet::GetPhysicalDefenseAttribute(), 5.f);
    DefaultStats.Add(UNGPawnAttributeSet::GetMagicDefenseAttribute(), 5.f);

    // =========================================================================
    // System / Utility Attributes (유틸리티/오토배틀러 관련)
    // =========================================================================
    DefaultStats.Add(UNGPawnAttributeSet::GetIncomeAttribute(), 10.f); // 돈 버는 수치
    DefaultStats.Add(UNGPawnAttributeSet::GetMoveSpeedAttribute(), 1.f); 
    DefaultStats.Add(UNGPawnAttributeSet::GetStarAttribute(), 1.f);         // 유닛 성급 (1성 기본)

}