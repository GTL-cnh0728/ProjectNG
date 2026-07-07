// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "NGItemInstance.generated.h"

class UNGItemDataAsset;
/**
 * 
 */

UENUM(BlueprintType)
enum class EItemModifierType : uint8
{
	Additive,
	Multiply,
	Override
};

USTRUCT()
struct FItemStatModifier
{
	GENERATED_BODY()
	
public:
	FItemStatModifier() : Value(0), ModifierOp(EItemModifierType::Additive)
	{
	}
	
	UPROPERTY(EditAnywhere)
	FGameplayAttribute ItemAttribute;
	
	UPROPERTY(EditAnywhere)
	float Value;
	
	UPROPERTY(EditAnywhere)
	EItemModifierType ModifierOp;

};

UCLASS()
class PROJECTNG_API UNGItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UNGItemInstance() : Count(1)
	{
	}
	
	//이게 없으면 TArray 복제가 안됨
	virtual bool IsSupportedForNetworking() const override{	return true; }
	
	virtual void InitializeItem(UNGItemDataAsset* InData);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	TObjectPtr<UNGItemDataAsset> ItemData;

	UPROPERTY(Replicated)
	int32 Count;
	
};
