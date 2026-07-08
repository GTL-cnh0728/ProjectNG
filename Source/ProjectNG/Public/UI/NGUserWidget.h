// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NGUserWidget.generated.h"

class UNGInventoryWidget;
/**
 * 
 */
UCLASS()
class PROJECTNG_API UNGUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* 위젯 컨트롤러 연결 */
	UFUNCTION(BlueprintCallable)
	void ConnectWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
	
protected:
	/* 위젯 컨트롤러가 연결 되었을 때, 위젯에서 자체적으로 데이터 처리를 위한 함수
	 * 
	 * **블루프린트 환경에서 함수 구성**
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void BindWidgetController();
};
