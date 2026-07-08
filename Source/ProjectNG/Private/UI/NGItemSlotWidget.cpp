// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/NGItemSlotWidget.h"

#include "Combat/Item/NGItemDataAsset.h"
#include "Combat/Item/NGItemInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Player/NGPlayerController.h"

void UNGItemSlotWidget::Init(UNGItemInstance* InItem)
{
	Item = InItem;
	
	Icon->SetBrushFromTexture(InItem->ItemData->Icon.LoadSynchronous());
	ItemName->SetText(InItem->ItemData->Name);
	ItemCount->SetText(FText::AsNumber(Item->Count));
}

FReply UNGItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bReadyToDrag = true;
		
		return FReply::Handled();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UNGItemSlotWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bReadyToDrag)
	{
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}
	
	if (ANGPlayerController* PC = GetOwningPlayer<ANGPlayerController>())
	{
		PC->SetDragItemWithUpdateUI(Item);
		
		bReadyToDrag = false;

		return FReply::Handled();
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);;
}