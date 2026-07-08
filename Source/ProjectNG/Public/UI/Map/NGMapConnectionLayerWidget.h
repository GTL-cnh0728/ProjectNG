// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/NGUserWidget.h"
#include "Map/NGMapTypes.h"
#include "NGMapConnectionLayerWidget.generated.h"

UCLASS()
class PROJECTNG_API UNGMapConnectionLayerWidget : public UNGUserWidget
{
	GENERATED_BODY()

public:
	void SetConnectionData(const TArray<FMapNodeData>& InMapData, const FLinearColor& InLineColor, float InLineThickness);

protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	TArray<FMapNodeData> CachedMapData;
	FLinearColor LineColor = FLinearColor(1.f, 1.f, 1.f, 0.5f);
	float LineThickness = 2.0f;
};
