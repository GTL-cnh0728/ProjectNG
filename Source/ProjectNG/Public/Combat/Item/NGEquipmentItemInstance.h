// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "NGEquipmentItemDataAsset.h"
#include "NGItemInstance.h"
#include "NGEquipmentItemInstance.generated.h"

class UNGEquipmentItemDataAsset;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGEquipmentItemInstance : public UNGItemInstance
{
	GENERATED_BODY()
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void InitializeItem(UNGItemDataAsset* InData) override;
	
	FORCEINLINE UNGEquipmentItemDataAsset* GetEquipDataAsset() const { return CastChecked<UNGEquipmentItemDataAsset>(ItemData); }
	
	TArray<FItemStatModifier> GenerateStatModifiers() const;

	UPROPERTY(Replicated)
	TArray<FItemStatModifier> StatRandomModifiers;
	
	FActiveGameplayEffectHandle AppliedHandle;
};
