// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Combat/Item/NGEquipmentItemInstance.h"

#include "Combat/Item/NGEquipmentItemDataAsset.h"
#include "Net/UnrealNetwork.h"

void UNGEquipmentItemInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UNGEquipmentItemInstance, StatRandomModifiers);
	
}

void UNGEquipmentItemInstance::InitializeItem(UNGItemDataAsset* InData)
{
	Super::InitializeItem(InData);
		
	StatRandomModifiers = GenerateStatModifiers();
}

TArray<FItemStatModifier> UNGEquipmentItemInstance::GenerateStatModifiers() const
{
	UNGEquipmentItemDataAsset* EquipData = GetEquipDataAsset();
	
	const int32 OptionCount = FMath::Min(
			FMath::RandRange(EquipData->MinOptionCount,	EquipData->MaxOptionCount),
			EquipData->RandomModifierStats.Num());

	TArray<FItemStatModifier> Modifiers;
	
	for (int CurrentOptionCount = 0; CurrentOptionCount < OptionCount; CurrentOptionCount++)
	{
		int32 RandomOptionIndex = FMath::RandRange(0, EquipData->RandomModifierStats.Num() - 1);
		const FEquipmentRandomStat& RandomData = EquipData->RandomModifierStats[RandomOptionIndex];
		
		FItemStatModifier StatModifier;
		StatModifier.ItemAttribute = RandomData.Attribute;
		StatModifier.Value = FMath::FRandRange(RandomData.MinValue, RandomData.MaxValue);
		
		Modifiers.Add(StatModifier);
	}
	
	return Modifiers;
}

