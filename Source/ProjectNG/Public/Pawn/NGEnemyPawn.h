// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/NGPawnBase.h"
#include "NGEnemyPawn.generated.h"

struct FEnemyAbilityData;
class UWidgetComponent;
class USplineComponent;

UCLASS()
class PROJECTNG_API ANGEnemyPawn : public ANGPawnBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANGEnemyPawn();

	virtual void Activate() override;
	void InitializeEnemyStats(const FUnitAbilityData& DataRow);

	virtual void OnRep_PlayerState() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Die() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Initialize(ANGPlayerState* PS) override;
	
private:
	virtual void InitAbilityActorInfo() override;
	
};
