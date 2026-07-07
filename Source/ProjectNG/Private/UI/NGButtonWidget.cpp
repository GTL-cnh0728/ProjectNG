// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "UI/NGButtonWidget.h"

#include "Components/TextBlock.h"

void UNGButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Text->SetText(ButtonText);

	Button->OnClicked.AddDynamic(this, &UNGButtonWidget::HandleButtonClicked);
}

void UNGButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (Text)
	{
		Text->SetText(ButtonText);
	}
}

void UNGButtonWidget::HandleButtonClicked()
{
	OnTabClicked.Broadcast(ButtonTag);
}

void UNGButtonWidget::SetSelected(bool bSelected) const
{
	Button->SetBackgroundColor(bSelected ? FLinearColor::White : FLinearColor::Gray);
}
