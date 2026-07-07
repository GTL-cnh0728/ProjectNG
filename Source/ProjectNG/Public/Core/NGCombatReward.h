#pragma once

#include "CoreMinimal.h"
#include "NGCombatReward.generated.h"

USTRUCT(BlueprintType)
struct FReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	int32 Gold;
};

