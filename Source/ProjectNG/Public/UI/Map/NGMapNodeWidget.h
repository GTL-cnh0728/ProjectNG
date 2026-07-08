// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/NGUserWidget.h"
#include "Map/NGMapTypes.h"
#include "Map/MapNodeDataAsset.h"
#include "NGMapNodeWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapNodeClickedSignature, int32, NodeID);

/**
 * 맵의 개별 노드를 나타내는 UI 위젯
 */
UCLASS()
class PROJECTNG_API UNGMapNodeWidget : public UNGUserWidget
{
	GENERATED_BODY()
	
public:
	// 노드 초기화
	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void InitializeNode(const FMapNodeData& InNodeData, const FMapNodeVisualData& InVisualData);

	// 클릭 이벤트
	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void HandleNodeClicked();

	UFUNCTION(BlueprintCallable, Category = "MapUI")
	void SetNodeSelectable(bool bInSelectable);

	// 클릭 이벤트 브로드캐스터
	UPROPERTY(BlueprintAssignable, Category = "MapUI")
	FOnMapNodeClickedSignature OnNodeClicked;

	// 노드 데이터
	UPROPERTY(BlueprintReadOnly, Category = "MapUI")
	FMapNodeData NodeData;

	// 시각화 데이터
	UPROPERTY(BlueprintReadOnly, Category = "MapUI")
	FMapNodeVisualData VisualData;

protected:
	// 블루프린트에서 UI 요소를 업데이트하기 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "MapUI")
	void UpdateVisuals();

	UFUNCTION(BlueprintImplementableEvent, Category = "MapUI")
	void UpdateAvailabilityVisual(bool bIsSelectable);
};
