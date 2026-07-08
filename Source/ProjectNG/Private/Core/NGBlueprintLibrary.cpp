// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Core/NGBlueprintLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/NGPlayerState.h"
#include "UI/NGWidgetController.h"
#include "UI/HUD/NGHUD.h"
#include "Game/NGPawnDataManager.h"

ANGHUD* UNGBlueprintLibrary::GetNGHUD(const UObject* WorldContextObject)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (PC)
	{
		return Cast<ANGHUD>(PC->GetHUD());
	}

	return nullptr;
}

bool UNGBlueprintLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetParams& OutWidgetParams,
	ANGHUD*& OutHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutHUD = Cast<ANGHUD>(PC->GetHUD());
		if (OutHUD)
		{
			ANGPlayerState* PS = PC->GetPlayerState<ANGPlayerState>();
			OutWidgetParams.PlayerState = PS;
			OutWidgetParams.PlayerController = PC;
			return true;
		}
	}
	return false;
}

UNGRollShopWidgetController* UNGBlueprintLibrary::GetRollShopWidgetController(const UObject* WorldContextObject)
{
	FWidgetParams WParams;
	ANGHUD* NGHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WParams, NGHUD))
	{
		return NGHUD->CreateRollShopWidgetController(WParams);
	}
	
	return nullptr;
}

UNGMapWidgetController* UNGBlueprintLibrary::GetMapWidgetController(const UObject* WorldContextObject)
{
	FWidgetParams WParams;
	ANGHUD* NGHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WParams, NGHUD))
	{
		return NGHUD->CreateMapWidgetController(WParams);
	}
	
	return nullptr;
}

UNGMainWidgetController* UNGBlueprintLibrary::GetMainWidgetController(const UObject* WorldContextObject)
{
	FWidgetParams WParams;
	ANGHUD* NGHUD = nullptr;
	
	if (MakeWidgetControllerParams(WorldContextObject, WParams, NGHUD))
	{
		return NGHUD->CreateMainWidgetController(WParams);
	}
	
	return nullptr;
}

void UNGBlueprintLibrary::GetUnitDataRowByTag(const UObject* WorldContextObject, FGameplayTag UnitTag, EGetDataTableRowResult& OutResult, FUnitData& OutRow)
{
	if (WorldContextObject)
	{
		if (UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance())
		{
			if (UNGPawnDataManager* UnitDataManager = GI->GetSubsystem<UNGPawnDataManager>())
			{
				if (const FUnitData* FoundData = UnitDataManager->GetUnitData(UnitTag))
				{
					OutRow = *FoundData;
					OutResult = EGetDataTableRowResult::RowFound;
					return;
				}
			}
		}
	}

	OutResult = EGetDataTableRowResult::RowNotFound;
}

void UNGBlueprintLibrary::GetUnitDataRowFromTableByTag(UDataTable* DataTable, FGameplayTag UnitTag, EGetDataTableRowResult& OutResult, FUnitData& OutRow)
{
	if (DataTable && UnitTag.IsValid())
	{
		for (auto& Pair : DataTable->GetRowMap())
		{
			FUnitData* RowData = (FUnitData*)Pair.Value;
			if (RowData && RowData->IdentificationTag == UnitTag)
			{
				OutRow = *RowData;
				OutResult = EGetDataTableRowResult::RowFound;
				return;
			}
		}
	}

	OutResult = EGetDataTableRowResult::RowNotFound;
}

FText UNGBlueprintLibrary::FormatSecondsAsMMSS(float TimeInSeconds)
{
	int32 TotalSeconds = FMath::FloorToInt(TimeInSeconds);
	int32 Minutes = TotalSeconds / 60;
	int32 Seconds = TotalSeconds % 60;
	
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}
