// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "NGButtonWidget.generated.h"

class UTextBlock;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabClicked, FGameplayTag, TabTag);


UCLASS()
class PROJECTNG_API UNGButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void HandleButtonClicked();
	
public:
	
	void SetSelected(bool bSelected) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	FText ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	FGameplayTag ButtonTag;
	
	UPROPERTY(BlueprintAssignable)
	FOnTabClicked OnTabClicked;

	UPROPERTY(meta=(BindWidget))
	UButton* Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text;
	
};
