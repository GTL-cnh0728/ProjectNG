// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "MapNodeDataAsset.generated.h"

class UTexture2D;

// 2D UI 렌더링을 위한 노드 시각화 데이터
USTRUCT(BlueprintType)
struct FMapNodeVisualData
{
	GENERATED_BODY()
	
	// 기준이 되는 노드 태그 (예: Node.Shop)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NodeVisual")
	FGameplayTag NodeTag;
	
	// UI에 표시될 노드 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NodeVisual")
	FText NodeName;
	
	// UI 아이콘
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NodeVisual")
	TObjectPtr<UTexture2D> NodeIcon = nullptr;
	
	// UI 색상
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NodeVisual")
	FLinearColor NodeColor = FLinearColor::White;
};

/**
 * 맵 노드의 시각화 데이터를 관리하는 데이터 에셋
 */
UCLASS(BlueprintType)
class PROJECTNG_API UMapNodeDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapNode")
	TArray<FMapNodeVisualData> MapNodeVisuals;
	
	UFUNCTION(BlueprintCallable, Category = "MapNode")
	bool GetMapNodeVisual(const FGameplayTag& Tag, FMapNodeVisualData& OutData) const;
};
