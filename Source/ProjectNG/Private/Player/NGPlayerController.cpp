// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Player/NGPlayerController.h"

#include "Components/NGPocketComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Combat/Grid/Arena.h"
#include "Combat/Item/NGItemFactory.h"
#include "Components/NGCombatManagerComponent.h"
#include "Components/NGInventoryComponent.h"
#include "Core/NGDeveloperSettings.h"
#include "Pawn/NGUnitPawn.h"
#include "Pawn/SelectableInterface.h"
#include "Core/NGBlueprintLibrary.h"
#include "Core/NGGameplayTags.h"
#include "Core/NGSpawnHelper.h"
#include "GameModes/NGInGameMode.h"
#include "Input/NGInputComponent.h"
#include "Player/NGPlayerState.h"

#include "ProjectNG/ProjectNG.h"
#include "UI/NGUnitInfoWidget.h"
#include "UI/HUD/NGHUD.h"
#include "UI/WidgetController/UnitDetailsWidgetController.h"

ANGPlayerController::ANGPlayerController() : DragThreshold(10.f), DragHeightOffset(20.f), DragInterpSpeed(20.f)
{
	UE_LOG(LogTemp, Warning, TEXT("---------------PC Created!---------------"));
	UE_LOG(LogTemp, Warning, TEXT("PC Addr: %p"), this);
}

ANGPlayerController::~ANGPlayerController()
{
	UE_LOG(LogTemp, Warning, TEXT("---------------PC Destroyed!---------------"));
}

void ANGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetIgnoreLookInput(true);
	
	bShowMouseCursor = true;
	bEnableClickEvents = true; 
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	CurrentClickTraceChannel = ECC_SelectableUnit;
	
	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewYawMin = -179.9f;
		PlayerCameraManager->ViewYawMax = 179.9f;
		PlayerCameraManager->ViewPitchMin = -89.9f;
		PlayerCameraManager->ViewPitchMax = 89.9f;
	}
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext.Get(), 0);
			}
		}
	}
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void ANGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// InputComponent를 EnhancedInputComponent로 캐스팅합니다.
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// IA_Click 액션을 C++ 함수와 바인딩합니다.
		if (ClickInputAction)
		{
			EnhancedInputComponent->BindAction(ClickInputAction, ETriggerEvent::Started, this, &ThisClass::HandleClickPressed);
			EnhancedInputComponent->BindAction(ClickInputAction, ETriggerEvent::Completed, this, &ThisClass::HandleClickReleased);
		}
	}
}

void ANGPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GetMousePosition(CurrentMouseLocation.X, CurrentMouseLocation.Y);
	
	if (IsLocalController())
	{
		if (DraggingUnit.IsValid())
		{
			PerformDragUpdate(DeltaTime);
		}
	
		if (CurrentDraggingItem)
		{
			if (WasInputKeyJustReleased(EKeys::LeftMouseButton))
			{
				OnItemDragReleased();
			}
		}
	}
	
	if (bShowDebugGrid)
	{
		if (ANGPlayerState* PS = GetPlayerState<ANGPlayerState>())
		{
			FGridAddress CombatGridAddress(FIntVector2::ZeroValue, EGridType::Combat, PS, 0);
			UGridMapHelper::DrawDebugGrid(this, CombatGridAddress);
			FGridAddress WaitGridAddress(FIntVector2::ZeroValue, EGridType::Wait, PS, 0);
			UGridMapHelper::DrawDebugGrid(this, WaitGridAddress);
		}
	}
}

void ANGPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
}

void ANGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void ANGPlayerController::PerformDragUpdate(float DeltaTime)
{
	if (!HasAuthority())
	{
		if (DraggingUnit.IsValid())
		{
			FHitResult HitResult;
			
			if (GetHitResultUnderCursor(ECC_Map, false, HitResult))
			{				
				FVector CurrentLocation = DraggingUnit->GetActorLocation();
				
				FVector TargetLocation = HitResult.Location;
				TargetLocation.Z += DragHeightOffset;
				
				FVector SmoothedLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, DragInterpSpeed);
				
				DraggingUnit->SetActorLocation(SmoothedLocation);

				if (AArena* Arena = Cast<AArena>(HitResult.GetActor()))
				{
					HighLightGrid(TargetLocation, Arena);
				}
			}else
			{
				ResetHighlight();
			}
		}
	}
}

void ANGPlayerController::ResetHighlight()
{
	if (PreHighlightGridAddress.IsSet())
	{
		const FGridAddress& GridAddress = PreHighlightGridAddress.GetValue();
		if (AArena* Arena = GridAddress.GridOwnerPS ? GridAddress.GridOwnerPS->GetHomeArena() : nullptr)
		{
			Arena->HighlightSpecificGrid(GridAddress, 0.f, true);
		}
	}
	
	PreHighlightGridAddress.Reset();
}

void ANGPlayerController::HighLightGrid(const FVector& TargetLocation, AArena* Arena)
{
	if (Arena)
	{
		if (ANGPlayerState* PS = Arena->GetOwnerPS())
		{
			EGridType GridType = UGridMapHelper::GetGridType(TargetLocation, PS);
			FIntVector2 GridIndex = UGridMapHelper::GetCellIndex(GridType, TargetLocation, PS);
						
			FGridAddress HighlightGridAddress(GridIndex, GridType, PS, 0);
			
			if (PreHighlightGridAddress.IsSet())
			{
				if (HighlightGridAddress == PreHighlightGridAddress.GetValue())
				{
					return;
				}
					
				ResetHighlight();
			}
			
			// 범위표시도 하려했는데 요구자원 많아서 굳이..? 싶은 만드려면 Range표시한자리 지나갔을때 치워주는것도 만들어야함
			// if (DraggingUnit.IsValid())
			// {
			// 	DraggingUnit->HighlightRangeIndicator(HighlightGridAddress);
			// }
			
			if (CanHighlight(HighlightGridAddress))
			{				
				CurrentHighlightLocation = TargetLocation;
				
				Arena->HighlightSpecificGrid(HighlightGridAddress, 1.f, true);
			
				PreHighlightGridAddress = HighlightGridAddress;
			}
		}
	}
}

bool ANGPlayerController::CanHighlight(const FGridAddress& GridAddress) const
{	
	ANGPlayerState* PS = GetPlayerState<ANGPlayerState>();
	
	EGameState GameState = PS ? PS->GetGameState() : EGameState::None;
	
	//TODO: 디버깅용으로 Exploration, 나중에 빼기
	if (GameState == EGameState::Combat || GameState == EGameState::Maintaining || GameState == EGameState::Exploration)
	{
		if (GridAddress.GridType == EGridType::Wait)
		{
			return true;
		}
		
		if (GameState == EGameState::Maintaining || GameState == EGameState::Exploration)
		{
			if (GridAddress.GridType == EGridType::Combat)
			{
				return 0 <= GridAddress.GridIndex.Y && GridAddress.GridIndex.Y < PS->GetCombatGridMap().Height / 2;
			}
		}
	}
	
	return false;
}

void ANGPlayerController::SetHoveringUnit(ANGPawnBase* InHoveringPawn)
{
	if (InHoveringPawn)
	{
		if (CanHighlight(InHoveringPawn->GetGridAddress()))
		{
			HoveringUnit = InHoveringPawn;
		}
	}
}

void ANGPlayerController::ClearHoveringUnit()
{
	HoveringUnit = nullptr;
}

ANGPawnBase* ANGPlayerController::GetHoveringUnit() const
{
	return HoveringUnit.Get();
}

void ANGPlayerController::SetDragItemWithUpdateUI(UNGItemInstance* InItem)
{
	if (HasAuthority())	return;
	
	SetDragItem(InItem);
	
	bool bIsDraggingItem = InItem ? true : false;

	if (const ANGHUD* NGHUD = GetHUD<ANGHUD>())
	{
		if (bIsDraggingItem)
		{
			NGHUD->ShowInventory(false);
		}else
		{
			NGHUD->ShowInventory(true);
		}
	}
}

void ANGPlayerController::SetDragItem(UNGItemInstance* InItem)
{
	CurrentDraggingItem = InItem;
}

void ANGPlayerController::OnItemDragReleased()
{
	UE_LOG(LogTemp, Log, TEXT("ANGPlayerController::OnItemDragReleased"));
	
	//아이템 드래그 관리
	if (CurrentDraggingItem)
	{
		if (HoveringUnit.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("EquipItem!"));
			ANGPlayerState* PS = GetPlayerState<ANGPlayerState>();
			if (UNGInventoryComponent* Inven = PS ? PS->GetPlayerInventory() : nullptr)
			{
				Inven->Server_EquipItem(HoveringUnit.Get(), CastChecked<UNGEquipmentItemInstance>(CurrentDraggingItem));
			}
		}
		
		//아이템 드래그중이면 끝났을떄 다시 인벤을 켜줘야하기 때문에 밖에서 호출하면 인벤이 꺼져있는상황에서도 mouse release상황에 인벤이 켜짐
		SetDragItemWithUpdateUI(nullptr);
	}
}

void ANGPlayerController::HandleClickPressed(const FInputActionValue& Value)
{
	GetMousePosition(ClickStartLocation.X, ClickStartLocation.Y);
	
	PerformDrag();
}

void ANGPlayerController::HandleClickReleased(const FInputActionValue& Value)
{
	//유닛 드래그 관리
	if (DraggingUnit.IsValid())
	{
		double MouseDelta = (ClickStartLocation - CurrentMouseLocation).Size();
	
		if (MouseDelta > DragThreshold)
		{
			DraggingUnit->TryMoveTo(CurrentHighlightLocation);
		}else
		{
			SetSelectedUnit(DraggingUnit.Get());
		}
		
		//원래 위치로 reject
		DraggingUnit->UpdatePawnCurrentLocation(DraggingUnit->GetGridAddress());
		ResetHighlight();
		ResetDragUnit();
		ResetHoveringUnit();
	}
}

void ANGPlayerController::SetSelectedUnit(ANGPawnBase* InSelectedUnit)
{
	ResetSelectUnit();
	
	SelectedUnit = InSelectedUnit;
	
	if (SelectedUnit)
	{
		if (SelectedUnit->Implements<USelectableInterface>())
		{
			ISelectableInterface::Execute_OnSelected(SelectedUnit.Get());
		}
		
		if (ANGHUD* MyNGHUD = GetHUD<ANGHUD>())
		{
			if (ANGUnitPawn* UnitPawn = Cast<ANGUnitPawn>(SelectedUnit.Get()))
			{
				if (UUnitDetailsWidgetController* DetailsWidgetController = MyNGHUD->GetUnitDetailsWidgetController())
				{
					DetailsWidgetController->SetTargetUnit(UnitPawn);
				}

				if (UNGUnitInfoWidget* UnitInfoWidget = MyNGHUD->GetUnitInfoWidget())
				{
					UnitInfoWidget->UpdateUnitWidget(UnitPawn);
				}
			}
		}
	}
}

void ANGPlayerController::ResetSelectUnit()
{
	if (SelectedUnit)
	{
		if (SelectedUnit->Implements<USelectableInterface>())
		{
			ISelectableInterface::Execute_OnDeselected(SelectedUnit.Get());
		}
		
		if (UnitInfoWidgetInstance)
		{
			UnitInfoWidgetInstance->ClearUnitDataOnUI();
			UnitInfoWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		if (UUnitDetailsWidgetController* UnitDetailsWidgetController = UNGBlueprintLibrary::GetUnitDetailsWidgetController(this))
		{
			UnitDetailsWidgetController->ClearTargetUnit();
		}
		
		SelectedUnit = nullptr;
	}
}

void ANGPlayerController::PerformDrag()
{
	if (!IsLocalController())	return;
	
	ResetDragUnit();
	ResetSelectUnit();
	
	if (HoveringUnit.IsValid())
	{
		//다른 유저거면 return
		if (HoveringUnit->GetOwner() != this)
		{
			return;
		}

		if (HoveringUnit->Implements<USelectableInterface>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HoveringUnit->GetName());
			
			DraggingUnit = Cast<ANGPawnBase>(HoveringUnit);
			ISelectableInterface::Execute_OnDrag(HoveringUnit.Get());
		}
	}
}

void ANGPlayerController::ResetDragUnit()
{
	if (DraggingUnit.IsValid())
	{
		if (DraggingUnit->Implements<USelectableInterface>())
		{
			ISelectableInterface::Execute_OnUndrag(DraggingUnit.Get());
		}
	
		DraggingUnit.Reset();
	}
}

void ANGPlayerController::ResetHoveringUnit()
{
	if (HoveringUnit.IsValid())
	{	
		HoveringUnit.Reset();
	}
}

void ANGPlayerController::Server_RequestSellUnit_Implementation(ANGUnitPawn* NewPawn)
{
	ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
	if (!GM)	return;
	
	if (ANGPlayerState* PS = GetPlayerState<ANGPlayerState>())
	{
		if (UNGPocketComponent* Pocket = PS->GetPlayerPocket())
		{
			float UnitPrice = GM->GetUnitPrice(NewPawn);
			PS->EarnGold(UnitPrice);
			Pocket->SellUnit(NewPawn);
			UE_LOG(LogTemp, Display, TEXT("SellUnitFromPocket Success"));
		}
	}
}

void ANGPlayerController::Server_RequestBuyUnit_Implementation(FGameplayTag UnitTag)
{
	if (ANGPlayerState* PS = GetPlayerState<ANGPlayerState>())
	{
		ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
		if (GM ? GM->CanBuyUnit(UnitTag, PS->GetOwnedGold()) : false)
		{
			if (ANGUnitPawn* NewPawn = UNGSpawnHelper::SpawnUnitPawn(this, UnitTag))
			{
				float UnitPrice = GM->GetUnitPrice(NewPawn);
				PS->EarnGold(-UnitPrice);
				Client_OnBuyUnit(true);
				
				UE_LOG(LogTemp, Display, TEXT("BuyUnitFromPocket Success"));
				return;
			}
		}
	}
	
	Client_OnBuyUnit(false);
}

void ANGPlayerController::Server_SelectNode_Implementation(int32 NodeID)
{
	if (ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>())
	{
		GM->ProcessNodeSelection(this, NodeID);
	}
}

void ANGPlayerController::Client_OnBuyUnit_Implementation(bool bIsSuccess)
{
	OnBuyUnitSuccess.Broadcast(bIsSuccess);
}

UNGPocketComponent* ANGPlayerController::GetPlayerPocket() const
{
	if (ANGPlayerState* PS = GetPlayerState<ANGPlayerState>())
	{
		return PS->GetPlayerPocket();
	}
	
	return nullptr;
}

void ANGPlayerController::Server_EnterPhase_Implementation(EGamePhase Phase)
{
	EnterPhase(Phase);
}

void ANGPlayerController::EnterPhase(EGamePhase Phase)
{
	if (!HasAuthority())	return;
	
	ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
	
	ANGPlayerState* PS = GetPlayerState<ANGPlayerState>();	
	
	if (UNGCombatManagerComponent* CMC = GM ? GM->GetCombatManagerComponent() : nullptr)
	{
		bool bIsCPUCombat = false;
		
		if (bIsCPUCombat)
		{
			TSoftObjectPtr<UNGEnemyDataAsset> SoftPath = GetDefault<UNGDeveloperSettings>()->EnemyDataAsset;

			if (SoftPath.IsNull()) return;

			UNGEnemyDataAsset* LoadedAsset = SoftPath.LoadSynchronous();
	    
			if (LoadedAsset)
			{
				FEnemySquadData SelectedData;
				if (LoadedAsset->GetRandomSquadForZone(PS->GetCurrentZoneTag(), SelectedData))
				{
					CMC->EnqueueCombatPhase(PS, &SelectedData);
				}
			}
		}
		else
		{
			CMC->EnqueueCombatPhase(PS);
		}
	}
}

void ANGPlayerController::Server_RequestFlee_Implementation()
{
	//도망갔을때 행동
	// CombatManager에서 나를 진사람, 상대방을 이긴사람으로 처리하고 게임 끝내기.
	// 내 돈의 10% 정도? 를 상대에게 헌납 (패널티 수행)
	ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>();
	
	if (UNGCombatManagerComponent* CMC = GM->GetCombatManagerComponent())
	{
		CMC->ProcessPlayerFlee(this);
	}
}

void ANGPlayerController::Server_RequestStopCombat_Implementation()
{
	if (ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>())
	{
		if (UNGCombatManagerComponent* CMC = GM->GetCombatManagerComponent())
		{
			CMC->FinishCombat();
		}
	}
}

void ANGPlayerController::Server_RequestStartCombat_Implementation(bool bIsCPUCombat)
{
	if (ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>())
	{
		GM->RequestStartCombat(this, bIsCPUCombat);
        
		// 확인용 로그
		UE_LOG(LogTemp, Warning, TEXT("Cmd: Wave Started!"));
        
		// 화면에 디버그 메시지 띄우기 (선택사항)
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Wave Started!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CombatManager가 GameState에 없습니다!"));
	}
}

void ANGPlayerController::Server_RequestToggleJohnAppeared_Implementation()
{
	ANGPlayerState* PS = GetPlayerState<ANGPlayerState>();
	UNGPocketComponent* Pocket = PS ? PS->GetPlayerPocket() : nullptr;
	if (Pocket)
	{
		Pocket->bDebugJohnAppeared = !Pocket->bDebugJohnAppeared;
	}
}

void ANGPlayerController::Server_RequestGetItem_Implementation(const FString& ItemName)
{
	ANGPlayerState* PS = GetPlayerState<ANGPlayerState>();
	UNGInventoryComponent* Inventory = PS ? PS->GetPlayerInventory() : nullptr;
	
	TMap<FString, FGameplayTag> CmdTags{
		{"Sword", NGGameplayTags::Item_Equipment_TestSword},
		{"Relic", NGGameplayTags::Item_Relic_TestRelic},
		{"Potion", NGGameplayTags::Item_Useable_TestPotion},
	};
	
	UNGItemInstance* NewItem = UNGItemFactory::CreateItem(Inventory, CmdTags[ItemName]);
	
	if (Inventory)
	{
		Inventory->AddItem(NewItem);
	}
}

void ANGPlayerController::Cmd_StartCombat(bool bIsCPUCombat)
{
	Server_RequestStartCombat(bIsCPUCombat);
}

void ANGPlayerController::Cmd_FinishCombat()
{
	Server_RequestStopCombat();
}

void ANGPlayerController::Cmd_ToggleDebugGrid()
{
	bShowDebugGrid = !bShowDebugGrid;
}

void ANGPlayerController::Cmd_ToggleJohn()
{
	Server_RequestToggleJohnAppeared();
}

void ANGPlayerController::Cmd_GetItem(const FString& ItemName)
{
	Server_RequestGetItem(ItemName);
}
