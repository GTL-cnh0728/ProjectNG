// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NGFloatingWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNGFloatingWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTNG_API INGFloatingWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void UpdateHP(float Percent);
	
	UFUNCTION(BlueprintNativeEvent, Category = "UI")
	void UpdateMP(float Percent);
};
