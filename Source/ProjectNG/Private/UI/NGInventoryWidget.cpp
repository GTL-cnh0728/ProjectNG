// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/NGInventoryWidget.h"

#include "Combat/Item/NGItemInstance.h"
#include "Components/NGInventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Player/NGPlayerController.h"
#include "UI/NGButtonWidget.h"
#include "UI/NGItemSlotWidget.h"

void UNGInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ConsumableTabButton->OnTabClicked.AddDynamic(this, &UNGInventoryWidget::ClickTab);
	EquipmentTabButton->OnTabClicked.AddDynamic(this, &UNGInventoryWidget::ClickTab);
	RelicTabButton->OnTabClicked.AddDynamic(this, &UNGInventoryWidget::ClickTab);
	
	RefreshInventory();
}

UNGInventoryComponent* UNGInventoryWidget::GetInventory()
{
	if (InventoryCache)	return InventoryCache.Get();
	
	ANGPlayerController* PC = GetOwningPlayer<ANGPlayerController>();
	
	if (ANGPlayerState* PS = PC ? PC->GetPlayerState<ANGPlayerState>() : nullptr)
	{
		InventoryCache = PS->GetPlayerInventory();
	}
	
	return InventoryCache.Get();
}

void UNGInventoryWidget::RefreshInventory()
{
	if (GetVisibility() == ESlateVisibility::Collapsed || GetVisibility() == ESlateVisibility::Hidden)
	{
		return;
	}
	
	ItemList->ClearChildren();
	
	UNGInventoryComponent* Inventory = GetInventory();
	
	const FInventoryList& Items = Inventory->GetItems();
	
	for(int32 i=0;i<Items.Items.Num();i++)
	{
		//탭의 태그를 포함안하면 컨티뉴
		UNGItemInstance* Item= Items.Items[i].Item;
		if (!Item->ItemData->ItemTag.MatchesTag(CurrentTab))	continue;
		
		UNGItemSlotWidget* MySlot = CreateWidget<UNGItemSlotWidget>(this, ItemSlotClass);

		MySlot->Init(Item);

		ItemList->AddChildToUniformGrid(
			MySlot,
			i/5,
			i%5);
	}
}

void UNGInventoryWidget::RefreshTabState()
{
	if (!ConsumableTabButton || !EquipmentTabButton || !RelicTabButton)	return;
	
	ConsumableTabButton->SetSelected(CurrentTab == ConsumableTabButton->ButtonTag);
	EquipmentTabButton->SetSelected(CurrentTab == EquipmentTabButton->ButtonTag);
	RelicTabButton->SetSelected(CurrentTab == RelicTabButton->ButtonTag);
}

void UNGInventoryWidget::ClickTab(FGameplayTag TabTag)
{
	CurrentTab = TabTag;
	
	RefreshTabState();
	
	RefreshInventory();
}
