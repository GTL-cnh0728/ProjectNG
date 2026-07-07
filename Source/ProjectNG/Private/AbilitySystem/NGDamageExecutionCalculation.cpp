// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "AbilitySystem/NGDamageExecutionCalculation.h"

#include "AbilitySystem/NGPawnAttributeSet.h"
#include "Core/NGDeveloperSettings.h"

struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AbilityPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DodgeRate);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicDefense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Star);
	
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, AttackDamage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, AbilityPower, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, CriticalRate, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, DodgeRate, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, PhysicalDefense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, MagicDefense, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UNGPawnAttributeSet, Star, Source, true);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UNGDamageExecutionCalculation::UNGDamageExecutionCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().AbilityPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().DodgeRateDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().MagicDefenseDef);
	RelevantAttributesToCapture.Add(DamageStatics().StarDef);
}

void UNGDamageExecutionCalculation::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	const UNGDeveloperSettings* Settings = GetDefault<UNGDeveloperSettings>();
	UDataTable* Table = Settings ? Settings->AttackLevelData.LoadSynchronous() : nullptr;
	if (!Table)	return;
	
	float Health = 0;
	float AttackDamage = 0;
	float AbilityPower = 0;
	float CriticalRate = 0;
	float DodgeRate = 0;
	float PhysicalDefense = 0;
	float MagicDefense = 0;
	float Star = 0;
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().HealthDef, FAggregatorEvaluateParameters(), Health);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDamageDef, FAggregatorEvaluateParameters(), AttackDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AbilityPowerDef, FAggregatorEvaluateParameters(), AbilityPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalRateDef, FAggregatorEvaluateParameters(), CriticalRate);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DodgeRateDef, FAggregatorEvaluateParameters(), DodgeRate);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PhysicalDefenseDef, FAggregatorEvaluateParameters(), PhysicalDefense);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().MagicDefenseDef, FAggregatorEvaluateParameters(), MagicDefense);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StarDef, FAggregatorEvaluateParameters(), Star);

	const FGameplayTagContainer& SkillTags = ExecutionParams.GetOwningSpec().GetDynamicAssetTags();

	if (SkillTags.IsEmpty())
	{
		return;
	}

	const FGameplayTag SkillTag = SkillTags.First();
	FName RowName(
		*FString::Format(TEXT("{0}_{1}"),
		{SkillTag.GetTagName().ToString(),FString::FromInt(static_cast<int32>(Star))}));
	UE_LOG(LogTemp, Log, TEXT("RowName: %s"), *RowName.ToString());
	FAttackLevelData* AttackData = Table->FindRow<FAttackLevelData>(RowName, TEXT(""));
	if (!AttackData)	return;
	
	float ADDamage = AbilityPower * AttackData->APMultiplier - PhysicalDefense;
	float APDamage = AttackDamage * AttackData->ADMultiplier - MagicDefense;
	
	float TotalDamage = AttackData->BaseDamage + (ADDamage < 0 ? 0 : ADDamage) + (APDamage < 0 ? 0 : APDamage);
	
	if(FMath::FRand() < CriticalRate)
	{
		TotalDamage *= 2;
	}
	
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
		UNGPawnAttributeSet::GetHealthAttribute(),
		EGameplayModOp::Additive,
		-TotalDamage));
}
