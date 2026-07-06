// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/NGItemSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Combat/Item/NGItemDataAsset.h"
#include "Combat/Item/NGItemInstance.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Core/NGItemDragDropOperation.h"
#include "Player/NGPlayerController.h"
#include "UI/HUD/NGHUD.h"

void UNGItemSlotWidget::Init(UNGItemInstance* InItem)
{
	Item = InItem;
	
	Icon->SetBrushFromTexture(InItem->ItemData->Icon.LoadSynchronous());
	ItemName->SetText(InItem->ItemData->Name);
	ItemCount->SetText(FText::AsNumber(Item->Count));
}

FReply UNGItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UNGItemSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	
	UNGItemDragDropOperation* Drag = NewObject<UNGItemDragDropOperation>();
	
	Drag->DragItem = Item;
	Drag->DefaultDragVisual = this;
	Drag->DefaultDragVisual->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	OutOperation = Drag;
	
	if (ANGPlayerController* PC = GetOwningPlayer<ANGPlayerController>())
	{
		PC->SetDragItemWithUpdateUI(Item);
	}	
}