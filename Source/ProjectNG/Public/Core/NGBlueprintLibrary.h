// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/NGUnitData.h"
#include "GameplayTagContainer.h"
#include "NGBlueprintLibrary.generated.h"

class UNGMainWidgetController;
class UNGMapWidgetController;
class UNGRollShopWidgetController;
struct FWidgetParams;
class UUnitDetailsWidgetController;
class ANGHUD;

UENUM(BlueprintType)
enum class EGetDataTableRowResult : uint8
{
	RowFound,
	RowNotFound
};

/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief: NGHUD를 반환합니다. 이 함수는 로컬에서만 실행해야 합니다. 멀티플레이어 환경에서는 서버에서 호출하면 안 됩니다.
	 * @param WorldContextObject: WorldContextObject는 일반적으로 Blueprint에서 "self"로 전달됩니다. 이 함수는 해당 객체의 월드 컨텍스트를 사용하여 플레이어 컨트롤러와 HUD에 접근합니다.
	 * @return: ANGHUD* - NGHUD의 포인터를 반환합니다.
	 * @note: 이 함수는 로컬 플레이어의 HUD에 접근하기 위한 유틸리티 함수입니다. 멀티플레이어 게임에서는 각 클라이언트가 자신의 HUD에 접근해야 하므로, 서버에서 이 함수를 호출하면 올바른 HUD에 접근할 수 없습니다. 따라서 이 함수는 클라이언트 측에서만 사용해야 합니다
	 */
	UFUNCTION(BlueprintPure, Category = "NG|Utility", meta = (DefaultToSelf = "WorldContextObject"))
	static ANGHUD* GetNGHUD(const UObject* WorldContextObject);

	/**
	 * 
	 * @param WorldContextObject: 일반적으로 Blueprint에서 "self"로 전달됩니다. 이 객체의 월드 컨텍스트를 사용하여 플레이어 컨트롤러와 HUD에 접근합니다.
	 * @param OutWidgetParams: FWidgetParams 구조체로, 플레이어 컨트롤러와 플레이어 상태에 대한 포인터를 포함합니다. 이 매개변수는 함수 내에서 채워집니다.
	 * @param OutHUD: ANGHUD의 포인터로, 함수 내에서 채워집니다. 이 매개변수는 플레이어의 HUD에 대한 포인터를 반환하는 데 사용됩니다.
	 * @return: bool - 함수가 성공적으로 플레이어 컨트롤러와 HUD에 접근하여 OutWidgetParams와 OutHUD를 채웠는지 여부를 반환합니다.
	 * @note: 이 함수는 플레이어 컨트롤러와 HUD에 접근하기 위한 유틸리티 함수입니다. 성공적으로 접근하면 OutWidgetParams와 OutHUD가 채워집니다.
	 * 멀티플레이어 게임에서는 각 클라이언트가 자신의 HUD에 접근해야 하므로, 서버에서 이 함수를 호출하면 올바른 HUD에 접근할 수 없습니다. 따라서 이 함수는 클라이언트 측에서만 사용해야 합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "NG|Utility", meta = (DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetParams& OutWidgetParams, ANGHUD*& OutHUD);
	
	UFUNCTION(BlueprintPure, Category = "NG|Utility", meta = (DefaultToSelf = "WorldContextObject"))
	static UNGRollShopWidgetController* GetRollShopWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "NG|Utility", meta = (DefaultToSelf = "WorldContextObject"))
	static UNGMapWidgetController* GetMapWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "NG|Utility", meta = (DefaultToSelf = "WorldContextObject"))
	static UNGMainWidgetController* GetMainWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "NG|Data", meta = (WorldContext = "WorldContextObject", ExpandEnumAsExecs = "OutResult"))
	static void GetUnitDataRowByTag(const UObject* WorldContextObject, FGameplayTag UnitTag, EGetDataTableRowResult& OutResult, FUnitData& OutRow);

	UFUNCTION(BlueprintCallable, Category = "NG|Data", meta = (ExpandEnumAsExecs = "OutResult"))
	static void GetUnitDataRowFromTableByTag(UDataTable* DataTable, FGameplayTag UnitTag, EGetDataTableRowResult& OutResult, FUnitData& OutRow);
	
	UFUNCTION(BlueprintPure, Category = "NG|Time")
	static FText FormatSecondsAsMMSS(float TimeInSeconds);
};
