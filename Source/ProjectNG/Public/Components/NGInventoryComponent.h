// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/Item/NGEquipmentItemInstance.h"
#include "Components/ActorComponent.h"
#include "NGInventoryComponent.generated.h"


class UNGBuffManagerComponent;
class UNGRelicItemInstance;
class ANGPawnBase;
class UNGItemInstance;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNGInventoryComponent();

	const TArray<TObjectPtr<UNGItemInstance>>& GetItems() const{ return Items; }
	
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
	
protected:
	
	UNGBuffManagerComponent* GetBuffManager();
	
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Internal_AddItem(UNGItemInstance* NewItem);
	void Internal_RemoveItem(UNGItemInstance* Item);
	
	UPROPERTY()
	UNGBuffManagerComponent* BuffManagerCache;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	TArray<TObjectPtr<UNGItemInstance>> Items;
};
