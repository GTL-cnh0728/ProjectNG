// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/NGWidgetComponent.h"
#include "NGFloatingBarWidgetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTNG_API UNGFloatingBarWidgetComponent : public UNGWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNGFloatingBarWidgetComponent();

	virtual FVector2D ModifyProjectedLocalPosition(const FGeometry& ViewportGeometry, const FVector2D& LocalPosition) override;
};
