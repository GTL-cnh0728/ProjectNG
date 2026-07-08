// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/NGGameState.h"
#include "UI/NGUserWidget.h"
#include "Map/NGMapTypes.h"
#include "NGMapScreenWidget.generated.h"

class UMapNodeDataAsset;
class UNGMapConnectionLayerWidget;
class UNGMapNodeWidget;
class UCanvasPanel;

UCLASS()
class PROJECTNG_API UNGMapScreenWidget : public UNGUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void BuildMapUI(const TArray<FMapNodeData>& MapData, UMapNodeDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void RefreshNodeAvailability();

	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void RollMovementDice();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "MapUI")
	void UpdateMovementTurnVisual(bool bIsMyTurn, int32 DiceResult);

private:
	UFUNCTION()
	void HandleNodeClicked(int32 NodeID);

	UFUNCTION()
	void HandleGameFlowChanged(EGameplayPhase CurrentPhase, int32 CurrentTurn, float PhaseStartServerTime,
		float PhaseDuration, float RemainingTime);

	UFUNCTION()
	void HandleMovementTurnChanged();

	void BindGameFlowEvent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapUI")
	FLinearColor LineColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapUI")
	float LineThickness = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapUI")
	TSubclassOf<UNGMapNodeWidget> MapNodeWidgetClass;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> MapCanvas;

private:
	TArray<FMapNodeData> CachedMapData;

	UPROPERTY(Transient)
	TObjectPtr<UNGMapConnectionLayerWidget> ConnectionLayerWidget;

	UPROPERTY(Transient)
	TMap<int32, TObjectPtr<UNGMapNodeWidget>> NodeWidgets;
};
