// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/NGShopControlWidget.h"

#include "Combat/Item/NGEquipmentItemInstance.h"
#include "Components/NGInventoryComponent.h"
#include "Components/TextBlock.h"
#include "Core/NGItemDragDropOperation.h"
#include "Pawn/NGPawnBase.h"
#include "Player/NGPlayerController.h"
#include "UI/NGButtonWidget.h"
#include "UI/NGInventoryWidget.h"
#include "UI/HUD/NGHUD.h"

bool UNGShopControlWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	UNGItemDragDropOperation* Drag = Cast<UNGItemDragDropOperation>(InOperation);
	if (!Drag)	return false;
	
	ANGPlayerController* PC = GetOwningPlayer<ANGPlayerController>();
	ANGPawnBase* Unit = PC ? PC->GetHoveringUnit() : nullptr;
	if (!Unit)	return false;
	
	UNGEquipmentItemInstance* EquipItem = Cast<UNGEquipmentItemInstance>(Drag->DragItem);
	if (!EquipItem)	return false;
	
	ANGPlayerState* PS = PC ? PC->GetPlayerState<ANGPlayerState>() : nullptr;
	UNGInventoryComponent* Inventory = PS ? PS->GetPlayerInventory() : nullptr;
	if (!Inventory)	return false;
	
	Inventory->Server_EquipItem(Unit, EquipItem);
	
	return true;
}

void UNGShopControlWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (FleeButton)
	{
		FleeButton->Button->OnClicked.AddDynamic(this, &UNGShopControlWidget::OnFleeButtonClicked);
	}
	
	if (InvenButton)
	{
		InvenButton->Button->OnClicked.AddDynamic(this, &UNGShopControlWidget::OnInvenButtonClicked);
	}
}

void UNGShopControlWidget::OnFleeButtonClicked()
{
	if (WidgetController)
	{
		if (ANGPlayerController* PC = GetOwningPlayer<ANGPlayerController>())
		{
			ANGPlayerState* PS = PC->GetPlayerState<ANGPlayerState>();
			if (PS ? PS->GetGameState() == EGameState::Combat : false)
			{
				PC->Server_RequestFlee();
			}
		}
	}
}

void UNGShopControlWidget::OnInvenButtonClicked()
{
	if (!InventoryWidget)	return;
	if(ANGHUD* NGHUD = GetOwningPlayer()->GetHUD<ANGHUD>())
	{
		NGHUD->ShowInventory(true);
	}
}

void UNGShopControlWidget::UpdateGoldText(float NewGold)
{
	if (GoldText)
	{
		FText GoldNumberText = FText::AsNumber(FMath::FloorToInt(NewGold));

		FText FinalGoldText = FText::Format(NSLOCTEXT("UI", "GoldFormat", "Gold: {0}"), GoldNumberText);
        
		GoldText->SetText(FinalGoldText);
	}
}
