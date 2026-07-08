// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/NGWidgetController.h"
#include "Map/NGMapTypes.h"
#include "NGMapWidgetController.generated.h"

class ANGGameState;
class UMapNodeDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapNodesReceivedSignature, const TArray<FMapNodeData>&, MapNodes);

/**
 * 맵 화면(WBP_MapScreen)에 데이터를 전달하고
 * 노드 클릭 등의 상호작용을 중개하는 WidgetController
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTNG_API UNGMapWidgetController : public UNGWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// 노드 클릭 시 Widget에서 호출
	UFUNCTION(BlueprintCallable, Category = "Game|Map")
	void HandleNodeSelected(int32 NodeID);

	// 맵 데이터가 준비되었을 때 Widget으로 전파하는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Game|Map")
	FOnMapNodesReceivedSignature OnMapNodesReceived;

	// 맵 노드 시각화 데이터 에셋 (HUD 또는 블루프린트에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game|Map")
	TObjectPtr<UMapNodeDataAsset> MapNodeDataAsset;

private:
	// GameState에서 맵 데이터를 읽어와 브로드캐스트
	void BroadcastMapData();

	// OnMapDataReady 바인딩용
	UFUNCTION()
	void OnMapDataReady();
};
