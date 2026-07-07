// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Components/NGFloatingBarWidgetComponent.h"

// Sets default values for this component's properties
UNGFloatingBarWidgetComponent::UNGFloatingBarWidgetComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

FVector2D UNGFloatingBarWidgetComponent::ModifyProjectedLocalPosition(const FGeometry& ViewportGeometry,
	const FVector2D& LocalPosition)
{
	FVector2D FinalScreenPos = Super::ModifyProjectedLocalPosition(ViewportGeometry, LocalPosition);
	
	FinalScreenPos.Y -= 40.0f; 

	return FinalScreenPos;
}
