// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "NGAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "Core/NGGameplayTags.h"

UNGAssetManager& UNGAssetManager::Get()
{
	check(GEngine);
	return *Cast<UNGAssetManager>(GEngine->AssetManager);
}

void UNGAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	UAbilitySystemGlobals::Get().InitGlobalData();
}
