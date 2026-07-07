// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "NGPawnAnimationSet.generated.h"

class USkeletalMesh;
class UAnimInstance;

UCLASS()
class PROJECTNG_API UNGPawnAnimationSet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", DisplayName = "스켈레톤")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual", DisplayName = "AnimBP")
	TSubclassOf<UAnimInstance> AnimBlueprintClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> AnimMap;

	UAnimMontage* FindMontageByTag(const FGameplayTag& InTag) const
	{
		if (const TObjectPtr<UAnimMontage>* FoundMontage = AnimMap.Find(InTag))
		{
			return *FoundMontage;
		}
		return nullptr;
	}
};
