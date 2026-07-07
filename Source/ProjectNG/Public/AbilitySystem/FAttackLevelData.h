#pragma once

#include "CoreMinimal.h"
#include "FAttackLevelData.generated.h"

USTRUCT(BlueprintType)
struct FAttackLevelData : public FTableRowBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ADMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float APMultiplier;
};
