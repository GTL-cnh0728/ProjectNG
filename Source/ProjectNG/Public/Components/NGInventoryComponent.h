// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Item/NGEquipmentItemInstance.h"
#include "Components/ActorComponent.h"
#include "Core/Net/FastArrayWrapper.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "NGInventoryComponent.generated.h"


class UNGBuffManagerComponent;
class UNGRelicItemInstance;
class ANGPawnBase;
class UNGItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChangedSignature);

USTRUCT()
struct FInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY()
	TObjectPtr<UNGItemInstance> Item;
};

USTRUCT()
struct FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FInventoryItem> Items;
	
	DECLARE_FAST_ARRAY_FUNCTIONS(FInventoryList, FInventoryItem, UNGItemInstance);
};

DECLARE_FAST_ARRAY_TRAITS(UNGItemInstance)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNGInventoryComponent();

	const FInventoryList& GetItems() const{ return InventoryItems; }
	
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
	void AddItem(UNGItemInstance* NewItem);
	void AddItem(UNGRelicItemInstance* NewItem);
	void RemoveItem(UNGItemInstance* Item);
	void RemoveItem(UNGRelicItemInstance* Item);
	bool UseItem(UNGItemInstance* Item);
	bool EquipItem(ANGPawnBase* Unit, UNGEquipmentItemInstance* Item);
	bool UnEquipItem(ANGPawnBase* Unit);

	UFUNCTION(Server, Reliable)
	void Server_EquipItem(ANGPawnBase* Unit, UNGEquipmentItemInstance* Item);
	UFUNCTION(Server, Reliable)
	void Server_UnEquipItem(ANGPawnBase* Unit);
	UFUNCTION(Server, Reliable)
	void Server_UseItem(UNGItemInstance* Item);
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory | Event")
	FOnInventoryChangedSignature OnInventoryChanged;
protected:
	
	UNGBuffManagerComponent* GetBuffManager();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Internal_AddItem(UNGItemInstance* NewItem);
	void Internal_RemoveItem(UNGItemInstance* Item);
	
	UFUNCTION()
	void OnRep_Items();
	
	UPROPERTY()
	UNGBuffManagerComponent* BuffManagerCache;
	
	//ReplicateUsing써서 Inventory Widget Refresh
	UPROPERTY(ReplicatedUsing=OnRep_Items, VisibleAnywhere)
	FInventoryList InventoryItems;
};
