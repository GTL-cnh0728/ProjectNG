// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGItemFactory.h"

#include "GameplayTagContainer.h"
#include "Combat/Item/NGEquipmentItemInstance.h"
#include "Combat/Item/NGItemDataAsset.h"
#include "Combat/Item/NGItemDB.h"
#include "Core/NGDeveloperSettings.h"

UNGItemInstance* UNGItemFactory::CreateItem(UObject* Outer, FGameplayTag ItemTag)
{
	const UNGDeveloperSettings* Settings = GetDefault<UNGDeveloperSettings>();

	if (!Settings) return nullptr;

	UNGItemDB* DB = Settings->ItemDataBase.LoadSynchronous();

	if (!DB) return nullptr;

	UNGItemDataAsset* Data = DB->Items.FindRef(ItemTag);

	if (!Data) return nullptr;
	
	UNGItemInstance* Item = Data->CreateInstance(Outer);
	if (!Item)	return nullptr;
	
	Item->InitializeItem(Data);
	
	UE_LOG(LogTemp, Warning,
	TEXT("NewItem=%p Name=%s Outer=%s"),
	Item,
	*GetNameSafe(Item),
	*GetNameSafe(Item->GetOuter()));
	
	return Item;
}
