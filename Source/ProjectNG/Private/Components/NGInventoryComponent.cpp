// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Components/NGInventoryComponent.h"

#include "Combat/Item/NGItemDataAsset.h"
#include "Combat/Item/NGItemInstance.h"
#include "Combat/Item/NGRelicItemInstance.h"
#include "Components/NGBuffManagerComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "Pawn/NGPawnBase.h"
#include "Player/NGPlayerController.h"


// Sets default values for this component's properties
UNGInventoryComponent::UNGInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	
	// ...
}

bool UNGInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryItem& InventoryItem : InventoryItems.Items)
	{
		UNGItemInstance* Item = InventoryItem.Item;
		
		if (Item)
		{
			WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UNGInventoryComponent::AddItem(UNGItemInstance* NewItem)
{
	Internal_AddItem(NewItem);
}

void UNGInventoryComponent::AddItem(UNGRelicItemInstance* NewItem)
{
	UNGBuffManagerComponent* BuffManager = GetBuffManager();
	if (!BuffManager)	return;
	
	BuffManager->AddBuff(NewItem->ItemData->UseGameplayEffect);

	Internal_AddItem(NewItem);
}

void UNGInventoryComponent::Internal_AddItem(UNGItemInstance* NewItem)
{
	InventoryItems.Add(NewItem);
	
	if (GetOwner()->HasAuthority())
	{
		AddReplicatedSubObject(NewItem);
	}
}

void UNGInventoryComponent::RemoveItem(UNGItemInstance* Item)
{
	Internal_RemoveItem(Item);
}

void UNGInventoryComponent::RemoveItem(UNGRelicItemInstance* Item)
{
	UNGBuffManagerComponent* BuffManager = GetBuffManager();
	if (!BuffManager)	return;
	
	BuffManager->RemoveBuff(Item->ItemData->UseGameplayEffect);

	Internal_RemoveItem(Item);
}

void UNGInventoryComponent::Internal_RemoveItem(UNGItemInstance* Item)
{
	InventoryItems.Remove(Item);
	
	RemoveReplicatedSubObject(Item);
}

void UNGInventoryComponent::OnRep_Items()
{
	OnInventoryChanged.Broadcast();
}

bool UNGInventoryComponent::UseItem(UNGItemInstance* Item)
{	
	UNGBuffManagerComponent* BuffManager = GetBuffManager();
	if (!BuffManager)	return false;
	
	BuffManager->AddBuff(Item->ItemData->UseGameplayEffect);
	
	RemoveItem(Item);
	
	return true;
}

bool UNGInventoryComponent::EquipItem(ANGPawnBase* Unit, UNGEquipmentItemInstance* Item)
{
	if (!InventoryItems.Contains(Item) || !Unit)	return false;

	if (!Unit->EquipItem(Item))
	{
		return false;
	}
	
	InventoryItems.Remove(Item);
	return true;
}

bool UNGInventoryComponent::UnEquipItem(ANGPawnBase* Unit)
{
	if(!Unit)	return false;

	InventoryItems.Append<UNGEquipmentItemInstance>(Unit->UnEquipItem());
	
	return true;
}

void UNGInventoryComponent::Server_UnEquipItem_Implementation(ANGPawnBase* Unit)
{
	UnEquipItem(Unit);
}

void UNGInventoryComponent::Server_EquipItem_Implementation(ANGPawnBase* Unit, UNGEquipmentItemInstance* Item)
{
	EquipItem(Unit, Item);
}

void UNGInventoryComponent::Server_UseItem_Implementation(UNGItemInstance* Item)
{
	UseItem(Item);
}

UNGBuffManagerComponent* UNGInventoryComponent::GetBuffManager()
{
	if (!BuffManagerCache)
	{
		ANGPlayerState* PS = GetOwner<ANGPlayerState>();
		BuffManagerCache = PS ? PS->GetBuffManager() : nullptr;
	}
	
	return BuffManagerCache;
}

// Called when the game starts
void UNGInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UNGInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UNGInventoryComponent, InventoryItems);
}

