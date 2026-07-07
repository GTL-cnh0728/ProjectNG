// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NGAttributeSet.h"
#include "NGPawnAttributeSet.generated.h"

UCLASS()
class PROJECTNG_API UNGPawnAttributeSet : public UNGAttributeSet
{
	GENERATED_BODY()

public:
    // 리플리케이션(네트워크 동기화) 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    
public:
    // =========================================================================
    // Health & Mana (Vital Attributes)
    // =========================================================================
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, Health)
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, MaxHealth)
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, Mana)
    UFUNCTION()
    void OnRep_Mana(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, MaxMana)
    UFUNCTION()
    void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Vital", ReplicatedUsing = OnRep_ManaRegeneration)
    FGameplayAttributeData ManaRegeneration;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, ManaRegeneration)
    UFUNCTION()
    void OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue);
    
    // =========================================================================
    // Stats (Primary Attributes)
    // =========================================================================
public:
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_Agility)
    FGameplayAttributeData Agility;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, Agility)
    UFUNCTION()
    void OnRep_Agility(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_AttackDamage)
    FGameplayAttributeData AttackDamage;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, AttackDamage)
    UFUNCTION()
    void OnRep_AttackDamage(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stats", ReplicatedUsing = OnRep_AbilityPower)
    FGameplayAttributeData AbilityPower;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, AbilityPower)
    UFUNCTION()
    void OnRep_AbilityPower(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_AttackRange)
    FGameplayAttributeData AttackRange;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, AttackRange)
    UFUNCTION()
    void OnRep_AttackRange(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_AttackSpeed)
    FGameplayAttributeData AttackSpeed;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, AttackSpeed)
    UFUNCTION()
    void OnRep_AttackSpeed(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_CriticalRate)
    FGameplayAttributeData CriticalRate;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, CriticalRate)
    UFUNCTION()
    void OnRep_CriticalRate(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_DodgeRate)
    FGameplayAttributeData DodgeRate;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, DodgeRate)
    UFUNCTION()
    void OnRep_DodgeRate(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_PhysicalDefense)
    FGameplayAttributeData PhysicalDefense;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, PhysicalDefense)
    UFUNCTION()
    void OnRep_PhysicalDefense(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_MagicDefense)
    FGameplayAttributeData MagicDefense;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, MagicDefense)
    UFUNCTION()
    void OnRep_MagicDefense(const FGameplayAttributeData& OldValue);
    
public:
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Income)
    FGameplayAttributeData Income;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, Income)
    UFUNCTION()
    void OnRep_Income(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, MoveSpeed)
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
    
    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Combat", ReplicatedUsing = OnRep_Star)
    FGameplayAttributeData Star;
    ATTRIBUTE_ACCESSORS(UNGPawnAttributeSet, Star)
    UFUNCTION()
    void OnRep_Star(const FGameplayAttributeData& OldValue);
};
