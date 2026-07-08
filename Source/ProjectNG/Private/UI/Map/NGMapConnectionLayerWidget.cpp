// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "UI/Map/NGMapConnectionLayerWidget.h"

void UNGMapConnectionLayerWidget::SetConnectionData(const TArray<FMapNodeData>& InMapData, const FLinearColor& InLineColor, float InLineThickness)
{
	CachedMapData = InMapData;
	LineColor = InLineColor;
	LineThickness = InLineThickness;
	InvalidateLayoutAndVolatility();
}

int32 UNGMapConnectionLayerWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 MaxLayer = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (CachedMapData.IsEmpty())
	{
		return MaxLayer;
	}

	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
	if (LocalSize.X <= 0.0f || LocalSize.Y <= 0.0f)
	{
		return MaxLayer;
	}

	TMap<int32, FVector2D> NodePositions;
	for (const FMapNodeData& Node : CachedMapData)
	{
		NodePositions.Add(Node.NodeID, FVector2D(Node.Location.X * LocalSize.X, Node.Location.Y * LocalSize.Y));
	}

	for (const FMapNodeData& Node : CachedMapData)
	{
		const FVector2D* StartPos = NodePositions.Find(Node.NodeID);
		if (!StartPos)
		{
			continue;
		}

		for (int32 ConnectedID : Node.ConnectedNodeIDs)
		{
			if (Node.NodeID >= ConnectedID)
			{
				continue;
			}

			const FVector2D* EndPos = NodePositions.Find(ConnectedID);
			if (!EndPos)
			{
				continue;
			}

			TArray<FVector2D> Points;
			const FVector2D Delta = *EndPos - *StartPos;
			const float Distance = Delta.Size();
			constexpr float CurveDistanceThreshold = 180.0f;

			if (Distance > CurveDistanceThreshold)
			{
				const FVector2D Direction = Delta / Distance;
				const FVector2D Perpendicular(-Direction.Y, Direction.X);
				const float CurveSign = ((Node.NodeID + ConnectedID) % 2 == 0) ? 1.0f : -1.0f;
				const float CurveOffset = FMath::Clamp(Distance * 0.12f, 18.0f, 48.0f) * CurveSign;
				const FVector2D ControlPoint = (*StartPos + *EndPos) * 0.5f + Perpendicular * CurveOffset;
				constexpr int32 SegmentCount = 12;

				for (int32 SegmentIndex = 0; SegmentIndex <= SegmentCount; ++SegmentIndex)
				{
					const float T = static_cast<float>(SegmentIndex) / static_cast<float>(SegmentCount);
					const float InvT = 1.0f - T;
					const FVector2D CurvePoint =
						(*StartPos) * InvT * InvT +
						ControlPoint * 2.0f * InvT * T +
						(*EndPos) * T * T;

					Points.Add(CurvePoint);
				}
			}
			else
			{
				Points.Add(*StartPos);
				Points.Add(*EndPos);
			}

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				MaxLayer,
				AllottedGeometry.ToPaintGeometry(),
				Points,
				ESlateDrawEffect::None,
				LineColor,
				true,
				LineThickness
			);
		}
	}

	return MaxLayer + 1;
}
