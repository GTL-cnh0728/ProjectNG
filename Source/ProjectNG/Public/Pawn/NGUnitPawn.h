// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SelectableInterface.h"
#include "Core/NGUnitData.h"
#include "Pawn/NGPawnBase.h"
#include "NGUnitPawn.generated.h"

class UNGItemInstance;

UCLASS()
class PROJECTNG_API ANGUnitPawn : public ANGPawnBase, public ISelectableInterface
{
	GENERATED_BODY()

public:
	ANGUnitPawn();
	
	virtual void BeginPlay() override;
	
	virtual void Activate() override;
	virtual void Deactivate() override;
	
	virtual void OnSelected_Implementation() override;
	virtual void OnDeselected_Implementation() override;

	virtual void OnDrag_Implementation() override;
	virtual void OnUndrag_Implementation() override;
	
	virtual void OnRep_PlayerState() override;
	
protected:
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeAttributes() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	float DragInterpSpeed = 15.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	float AcceptanceRadius;
};
