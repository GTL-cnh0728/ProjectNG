// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "UI/Map/NGMapNodeWidget.h"

void UNGMapNodeWidget::InitializeNode(const FMapNodeData& InNodeData, const FMapNodeVisualData& InVisualData)
{
	NodeData = InNodeData;
	VisualData = InVisualData;
	
	UpdateVisuals();
}

void UNGMapNodeWidget::HandleNodeClicked()
{
	OnNodeClicked.Broadcast(NodeData.NodeID);
}

void UNGMapNodeWidget::SetNodeSelectable(bool bInSelectable)
{
	SetIsEnabled(bInSelectable);
	UpdateAvailabilityVisual(bInSelectable);
}
