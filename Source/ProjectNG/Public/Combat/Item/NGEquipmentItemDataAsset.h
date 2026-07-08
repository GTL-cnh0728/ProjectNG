// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "NGItemDataAsset.h"
#include "NGEquipmentItemDataAsset.generated.h"

class UNGGameplayAbility;
class UNGEquipmentItemInstance;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FEquipmentRandomStat
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FGameplayAttribute Attribute;
	
	UPROPERTY(EditAnywhere)
	float MinValue = 0;
	
	UPROPERTY(EditAnywhere)
	float MaxValue = 0;
};

UCLASS()
class PROJECTNG_API UNGEquipmentItemDataAsset : public UNGItemDataAsset
{
	GENERATED_BODY()
	
public:
	virtual UNGItemInstance* CreateInstance(UObject* Outer) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNGGameplayAbility> EquipAbility;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EquipGameplayEffect;
	
	UPROPERTY(EditAnywhere)
	TArray<FEquipmentRandomStat> RandomModifierStats;
	
	UPROPERTY(EditAnywhere)
	int32 MinOptionCount;
	UPROPERTY(EditAnywhere)
	int32 MaxOptionCount;
	
};
