// Copyright (c) 2025 TeamNG. All Rights Reserved.

#include "Pawn/NGPawnBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/NGAbilitySystemComponent.h"
#include "AbilitySystem/NGGameplayAbility.h"
#include "AbilitySystem/NGPawnAttributeSet.h"
#include "AbilitySystem/NGPlayerAttributeSet.h"
#include "Combat/Grid/Arena.h"
#include "Combat/Item/NGEquipmentItemDataAsset.h"
#include "Combat/Item/NGEquipmentItemInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/NGFloatingBarWidgetComponent.h"
#include "Components/NGPathFindingComponent.h"
#include "Components/NGPocketComponent.h"
#include "Components/WidgetComponent.h"
#include "Game/NGPawnDataManager.h"
#include "Core/NGDeveloperSettings.h"
#include "Core/NGPawnAnimationSet.h"
#include "Engine/ActorChannel.h"
#include "GameModes/NGInGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Pawn/NGUnitPawn.h"
#include "Player/NGPlayerController.h"
#include "ProjectNG/ProjectNG.h"
#include "UI/NGFloatingWidgetInterface.h"

ANGPawnBase::ANGPawnBase() : EquipMaxCount(3), SpeedScale(100.f), RotationInterpSpeed(10.f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;		//네트워크 복제 활성화
	SetNetUpdateFrequency(66.f);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComp"));

	CapsuleComponent->SetupAttachment(RootComponent);
    
	CapsuleComponent->InitCapsuleSize(40.f, 80.f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_SelectableUnit, ECR_Block);

	FVector CapsuleLocation = CapsuleComponent->GetRelativeLocation();
	CapsuleLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	CapsuleComponent->SetRelativeLocation(CapsuleLocation);

	UnitMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(CapsuleComponent);
    
	UnitMesh->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	UnitMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	UnitMesh->SetReceivesDecals(false);
	
	AbilitySystemComponent = CreateDefaultSubobject<UNGAbilitySystemComponent>(TEXT("Ability System Component"));

	AttributeSet = CreateDefaultSubobject<UNGPawnAttributeSet>(TEXT("AttributeSet"));
	
	PathFindingComponent = CreateDefaultSubobject<UNGPathFindingComponent>(TEXT("PathFindingComp"));
	
	FloatingBarComponent = CreateDefaultSubobject<UNGFloatingBarWidgetComponent>(TEXT("FloatingBarComponent"));
	FloatingBarComponent->SetupAttachment(CapsuleComponent);
	FVector UIRelativeLocation = FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FloatingBarComponent->SetRelativeLocation(UIRelativeLocation);
	FloatingBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	FloatingBarComponent->SetDrawSize(FVector2D(100.f, 30.f));
}

UAbilitySystemComponent* ANGPawnBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANGPawnBase::Multi_PlayMontage_Implementation(UAnimMontage* MontageToPlay)
{
	UE_LOG(LogTemp, Log, TEXT("Multi PlayMontage"));
	PlayAnimMontage(MontageToPlay);
}

float ANGPawnBase::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	UAnimInstance* AnimInstance = (UnitMesh) ? UnitMesh->GetAnimInstance() : nullptr;
    
	if (AnimMontage && AnimInstance)
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->Montage_Stop(0.2f);
		}
		
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// 특정 섹션부터 시작해야 한다면 점프
			if (StartSectionName != NAME_None)
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}
			return Duration;
		}
	}

	return 0.f;
}

void ANGPawnBase::StopAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* AnimInstance = (UnitMesh) ? UnitMesh->GetAnimInstance() : nullptr;

	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		if (AnimMontage)
		{
			// 몽타주 설정에 저장된 기본 블렌드 아웃 시간을 사용하여 부드럽게 중지
			AnimInstance->Montage_Stop(AnimMontage->GetDefaultBlendOutTime(), AnimMontage);
		}
		else
		{
			// 현재 재생 중인 모든 몽타주를 0.25초 동안 서서히 멈춤
			AnimInstance->Montage_Stop(0.25f);
		}
	}
}

void ANGPawnBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ANGPawnBase, CurrentGridAddress);
	DOREPLIFETIME(ANGPawnBase, NextGridPoint);
	DOREPLIFETIME(ANGPawnBase, PawnState);
	DOREPLIFETIME(ANGPawnBase, IdentificationTag);
	DOREPLIFETIME(ANGPawnBase, AnimationSet);
	DOREPLIFETIME(ANGPawnBase, EquipmentItems);
}

bool ANGPawnBase::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UNGEquipmentItemInstance* Item : EquipmentItems)
	{
		if (Item)
		{
			WroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void ANGPawnBase::HandleGameplayCue(UObject* Self, FGameplayTag GameplayCueTag, EGameplayCueEvent::Type EventType,
                                    const FGameplayCueParameters& Parameters)
{
	IGameplayCueInterface::HandleGameplayCue(Self, GameplayCueTag, EventType, Parameters);
}

void ANGPawnBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	InitAbilityActorInfo();
}

void ANGPawnBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitAbilityActorInfo();
}

void ANGPawnBase::Activate()
{
	ensureMsgf(IdentificationTag.IsValid(), TEXT("UnitIdTag is not valid for %s"), *GetName());
	
	if (HasAuthority())
	{
		Multicast_Activate();

		if (UNGPawnDataManager* UnitDataManager = GetWorld()->GetGameInstance()->GetSubsystem<UNGPawnDataManager>())
		{
			if (const FUnitAbilityData* UnitData = UnitDataManager->GetUnitAbilityData(IdentificationTag))
			{
				InitAbilityData(*UnitData);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[%s] 데이터 테이블에서 태그(%s)를 찾을 수 없습니다!"), *GetName(), *IdentificationTag.ToString());
			}
		}
	}
	
	if (!HasAuthority())
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}

void ANGPawnBase::Deactivate()
{
	if (HasAuthority())
	{
		Multicast_Deactivate();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
}

void ANGPawnBase::Multicast_Activate_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ANGPawnBase::Multicast_Deactivate_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ANGPawnBase::BeginPlay()
{
	Super::BeginPlay();
		
	if (AbilitySystemComponent)
	{
		InitializeAttributes();
	}

	UpdateHPBar();
	UpdateMPBar();
}

void ANGPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// VisualizePath();
	
	if (HasAuthority())
	{
		CheckCombatState();
	}
	
	if (!HasAuthority())
	{
		if (PawnState == EPawnState::Following)
		{
			VisualizeFollowing(DeltaTime);
		}
		else if (PawnState == EPawnState::Combat)
		{
			LookAtInterp(CurrentTarget, DeltaTime);
		}
	}
}

void ANGPawnBase::LookAtInterp(ANGPawnBase* Target, float DeltaTime)
{
	if (Target)
	{
		FVector Direction = Target->GetActorLocation() - GetActorLocation();
		FRotator TargetRotation = Direction.Rotation();
			
		//Yaw만 사용
		TargetRotation.Pitch = 0.f;
		TargetRotation.Roll = 0.f;
			
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed);
		SetActorRotation(NewRotation);
	}	
}

void ANGPawnBase::CheckCombatState()
{
	if (!HasAuthority())	return;
	
	if (CurrentTarget ? CurrentTarget->IsDead() : false)
	{
		TransitionToState(EPawnState::Wait);
		CurrentTarget = nullptr;
	}
	
	if (PawnState == EPawnState::Combat)
	{
		//싸우다가 적이 범위를 벗어나면 다시찾기
		if (!IsCurrentTargetInRange())
		{
			TransitionToState(EPawnState::Wait);
		}
	}
	else if (PawnState == EPawnState::Following)
	{
		if (IsCurrentTargetInRange())
		{
			TransitionToState(EPawnState::Combat);
		}
	}
	else if (PawnState == EPawnState::Wait)
	{
		FindNewTarget();
	}
}

float ANGPawnBase::GetMoveSpeed() const
{
	float MoveSpeed = AttributeSet ? AttributeSet->GetMoveSpeed() : 1.f;
	return MoveSpeed * SpeedScale;	
}

void ANGPawnBase::ConsiderTransitionState()
{
	//찾아갈길이 없으면 바로 앞에 있단 뜻
	if (TargetPath.Num() == 0)
	{
		TransitionToState(EPawnState::Combat);
	}else
	{
		ForceTransitionToState(EPawnState::Following);
	}
}

void ANGPawnBase::OnReachedNextGrid()
{
	// UE_LOG(LogTemp, Log, TEXT("OnReach index: %s"), *NextGridPoint.ToString());
	
	//도착시 우선 그리드 업데이트 및 현재 길찾기 상황변동 파악
	FGridMapBase* GridMap = UGridMapHelper::GetGridMap(CurrentGridAddress);
	if (!GridMap)	return;
	
	FGridAddress NextGridAddress = CurrentGridAddress;
	NextGridAddress.GridIndex = NextGridPoint;
	TranslatePawnOnGrid(NextGridAddress);
	
	GridMap->GridInfo[GridMap->ConvertPointToIndex(NextGridPoint)].bAnyoneIsComing = false;
	
	//적에게 도착시 전투상태로 이전
	if (!CurrentTarget || CurrentTarget->IsDead())
	{
		FindNewTarget();
		return;
	}
	
	int32 AttackRange = AttributeSet ? AttributeSet->GetAttackRange() : 1;
	if (UGridMapHelper::GetDistance(CurrentGridAddress.GridIndex, CurrentTarget->CurrentGridAddress.GridIndex) <= AttackRange 
		|| ++CurrentPathIndex >= TargetPath.Num())
	{
		TransitionToState(EPawnState::Combat);
		return;
	}
	
	//쫓던 적이 이동했으면 다시 찾기
	if (CurrentTarget && CurrentTarget->GetGridAddress().GridIndex != TargetLastIndex)
	{
		ReFindPath();
		return;
	}
	
	FIntVector2 NextGrid = TargetPath[CurrentPathIndex];
	if (GridMap->CanGoGrid(NextGrid))
	{
		ReFindPath();
		return;
	}
	
	//아무일 없으면 길따라 가기
	SetNextGridPoint(NextGrid);
}

void ANGPawnBase::ReFindPath()
{
	PathFindingComponent->FindPath(CurrentGridAddress, CurrentTarget->GetGridAddress(), TargetPath);
	InitializeFindNewPath();
}

void ANGPawnBase::FindNewTarget()
{
	if (ANGPawnBase* NewTarget = PathFindingComponent->FindPathToClosestEnemy(CurrentGridAddress, OwnerIndex, TargetPath))
	{
		CurrentTarget = NewTarget;
		
		InitializeFindNewPath();
		ConsiderTransitionState();
	}
}

void ANGPawnBase::InitializeFindNewPath()
{
	if (TargetPath.IsEmpty())	return;
	
	CurrentPathIndex = 0;
	TargetLastIndex = CurrentTarget->CurrentGridAddress.GridIndex;
	SetNextGridPoint(TargetPath[CurrentPathIndex]);
}

void ANGPawnBase::ForceTransitionToState(EPawnState NewState)
{
	//강제로 None으로 만들어서 무조건 TransitionToState수행
	PawnState = EPawnState::None;
	
	TransitionToState(NewState);
}

void ANGPawnBase::TransitionToState(EPawnState NewState)
{
	if (PawnState == NewState)	return;
	
	OnExitCurrentState(PawnState);
	
	PawnState = NewState;
	
	OnEnterNewState(NewState);
}

void ANGPawnBase::SetAttackCheckTimer(bool bRun) const
{
	if (bRun)
	{
		GetWorld()->GetTimerManager().UnPauseTimer(AttackCheckTimerHandle);
	}else
	{
		GetWorld()->GetTimerManager().PauseTimer(AttackCheckTimerHandle);
	}
}

void ANGPawnBase::OnApplyHardCrowdControl() const
{
	SetAttackCheckTimer(false);
	GetWorld()->GetTimerManager().PauseTimer(PredictGridReachingTimerHandle);
}

void ANGPawnBase::OnRemoveHardCrowdControl() const
{
	SetAttackCheckTimer(true);
	GetWorld()->GetTimerManager().UnPauseTimer(PredictGridReachingTimerHandle);
}

void ANGPawnBase::OnExitCurrentState(EPawnState RestState)
{
	if (RestState == EPawnState::Combat)
	{
		GetWorldTimerManager().ClearTimer(AttackCheckTimerHandle);
		CurrentTarget = nullptr;
	}else if (RestState == EPawnState::HardCrowdControl)
	{
		OnRemoveHardCrowdControl();
	}
}

void ANGPawnBase::OnEnterNewState(EPawnState EnteringState)
{
	if (EnteringState == EPawnState::Combat)
	{
		// UE_LOG(LogTemp, Log, TEXT("OnEnterCombatState"));
		GetWorld()->GetTimerManager().ClearTimer(PredictGridReachingTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(AttackCheckTimerHandle, this, &ANGUnitPawn::CheckAttackCondition, 0.2f, true);
	}else if (EnteringState == EPawnState::HardCrowdControl)
	{
		OnApplyHardCrowdControl();
	}else if (EnteringState == EPawnState::None)
	{
		RestoreStates();
	}
}

void ANGPawnBase::VisualizeFollowing(float DeltaTime)
{
	//클라이언트에서만 실행됨
	if (HasAuthority())	return;
	
	FVector CurrentLocation = GetActorLocation();
	float MoveSpeed = GetMoveSpeed();
	
	FGridAddress NextGridAddress = CurrentGridAddress;
	NextGridAddress.GridIndex = NextGridPoint;
	FVector NextGridLocation = UGridMapHelper::GetWorldLocation(NextGridAddress);
	
	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, NextGridLocation, DeltaTime, MoveSpeed);
	SetActorLocation(NewLocation);
	
	FVector MoveDirection = (NextGridLocation - CurrentLocation).GetSafeNormal2D();
	if (!MoveDirection.IsNearlyZero())
	{
		FRotator TargetRotation = MoveDirection.Rotation();
		FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationInterpSpeed * MoveSpeed);
		SetActorRotation(NewRotation);
	}
	
	float DistanceSq = FVector::DistSquared(NextGridLocation, NewLocation);
	if (DistanceSq < 1.f)
	{
		SetActorLocation(NextGridLocation);
	}
}

void ANGPawnBase::SetNextGridPoint(FIntVector2 NewNextGridPoint)
{
	if (!HasAuthority()) return;
	
	if (TargetPath.Num() == 0 || CurrentPathIndex >= TargetPath.Num()) return;

	NextGridPoint = NewNextGridPoint;
	
	// FVector CurrentLoc = UGridMapHelper::GetWorldLocation(CurrentGridAddress);
	FVector CurrentLoc = GetActorLocation();
    
	FGridAddress NextGridAddress = CurrentGridAddress;
	NextGridAddress.GridIndex = NextGridPoint;
	FVector NextGridLoc = UGridMapHelper::GetWorldLocation(NextGridAddress);

	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(NextGridAddress))
	{
		GridMap->GridInfo[GridMap->ConvertPointToIndex(NextGridAddress.GridIndex)].bAnyoneIsComing = true;
	}
	
	float ActualDistance = FVector::Distance(CurrentLoc, NextGridLoc);
    
	float MoveSpeed = GetMoveSpeed();
	if (MoveSpeed <= 0.f) MoveSpeed = 1.f; 

	float TimeToReach = ActualDistance / MoveSpeed;

	// UE_LOG(LogTemp, Log, TEXT("Current: %s Next: %s TimeToReach: %f"), *CurrentLoc.ToString(), *NextGridLoc.ToString(), TimeToReach);
	
	GetWorldTimerManager().SetTimer(PredictGridReachingTimerHandle, this, &ANGPawnBase::OnReachedNextGrid, TimeToReach, false);
}

bool ANGPawnBase::IsCurrentTargetInRange() const
{
	if (!IsValid(CurrentTarget))	return false;
		
	int32 Distance = UGridMapHelper::GetDistance(CurrentGridAddress.GridIndex, CurrentTarget->CurrentGridAddress.GridIndex);
	int32 AttackRange = AttributeSet ? AttributeSet->GetAttackRange() : 1;
	
	// UE_LOG(LogTemp, Log, TEXT("Distance %d, AttackRange %d"), Distance, AttackRange);
	
	if (Distance > AttackRange)
	{
		return false;
	}
	
	return true;

}

void ANGPawnBase::CollectInRangeUnits(TArray<ANGPawnBase*>& OutEnemies)
{
	//TODO: 나중에 여러명 한번에 때리는거 있으면 이거 쓰기
	
	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(CurrentGridAddress))
	{
		uint32 AttackRange = AttributeSet ? AttributeSet->GetAttackRange() : 1;

		TArray<FIntVector2> Neighbors;
		UGridMapHelper::GetHexNeighborIndexInRange(CurrentGridAddress.GridIndex, AttackRange, Neighbors, GridMap);
		
		for (FIntVector2 NeighborIndex : Neighbors)
		{
			if (!GridMap->IsValidIndex(NeighborIndex))	continue;
			
			uint32 ConvertIndex = GridMap->ConvertPointToIndex(NeighborIndex);
			ANGPawnBase* FindPawn = GridMap->GridInfo[ConvertIndex].PlacedPawn;
			if (IsValid(FindPawn))
			{
				OutEnemies.Add(FindPawn);
			}
		}
	}
}

void ANGPawnBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdateHPBar();
}

void ANGPawnBase::OnManaChanged(const FOnAttributeChangeData& Data)
{
	UpdateMPBar();
}

void ANGPawnBase::OnAttackRangeChanged(const FOnAttributeChangeData& Data)
{
}

void ANGPawnBase::InitializeAttributes()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetHealthAttribute()).AddUObject(this, &ANGPawnBase::OnHealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetManaAttribute()).AddUObject(this, &ANGPawnBase::OnManaChanged);
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetAttackRangeAttribute()).AddUObject(this, &ANGPawnBase::OnAttackRangeChanged);
	}
}

void ANGPawnBase::Initialize(ANGPlayerState* PS)
{
	OwnerIndex = PS->GetUserIndex();
}

void ANGPawnBase::Die()
{
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(DeadTag))	return;
	
	//PocketComponent가져와서 거기에 있는 OwnedPocket에서 자기자신 Remove
	ANGPlayerController* PC = GetOwner<ANGPlayerController>();
	ANGPlayerState* PS = PC ? PC->GetPlayerState<ANGPlayerState>() : nullptr;
	if (UNGPocketComponent* Pocket = PS->GetPlayerPocket())
	{
		Pocket->RemoveUnitFromPocket(this);
	}
	
	UnSetPawnOnGrid(CurrentGridAddress);
	
	//움직이던 도중에 죽으면 가려고 했던 예정지 예약취소
	if (TargetPath.Num() != 0)
	{
		if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(CurrentGridAddress))
		{
			GridMap->GridInfo[GridMap->ConvertPointToIndex(NextGridPoint)].bAnyoneIsComing = false;
		}
	}
	
	// AddLooseGameplayTag는 메모리 상에서만 일시적 태그를 붙일 때 유용
	GetAbilitySystemComponent()->AddLooseGameplayTag(DeadTag);
	
	Deactivate();
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true); //레그돌
	
	if (ANGInGameMode* GM = GetWorld()->GetAuthGameMode<ANGInGameMode>())
	{
		GM->ReportPawnDeath(this);
	}
	
	SetLifeSpan(1.0f);
	
	UE_LOG(LogTemp, Log, TEXT("%s has died"), *GetName());
}

bool ANGPawnBase::IsDead()
{
	if (AbilitySystemComponent)
	{
		float CurrentHP = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetHealthAttribute());
		
		return CurrentHP <= 0.f;
	}
	
	return true;
}

ANGPawnBase* ANGPawnBase::GetCurrentTarget()
{
	return CurrentTarget;
}

void ANGPawnBase::RestoreStates()
{
	CurrentTarget = nullptr;
	
	UE_LOG(LogTemp, Log, TEXT("RestoreState"));
	
	GetWorld()->GetTimerManager().ClearTimer(AttackCheckTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(PredictGridReachingTimerHandle);
	
	TransitionToState(EPawnState::None);
}

void ANGPawnBase::UpdateHPBar() const
{
	if (UUserWidget* FloatingWidget = FloatingBarComponent->GetUserWidgetObject())
	{
		float CurrentHP = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetHealthAttribute());
		float MaxHP = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetMaxHealthAttribute());
		
		float Percent = (MaxHP > 0.f) ? CurrentHP / MaxHP : 0.f;
		
		if (FloatingWidget->GetClass()->ImplementsInterface(UNGFloatingWidgetInterface::StaticClass()))
		{
			INGFloatingWidgetInterface::Execute_UpdateHP(FloatingWidget, Percent);
		}		
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't update HPBar"));
	}
}

void ANGPawnBase::UpdateMPBar() const
{
	if (UUserWidget* FloatingWidget = FloatingBarComponent->GetUserWidgetObject())
	{
		float CurrentMP = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetManaAttribute());
		float MaxMP = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetMaxManaAttribute());
		
		float Percent = (MaxMP > 0.f) ? CurrentMP / MaxMP : 0.f;
		
		if (FloatingWidget->GetClass()->ImplementsInterface(UNGFloatingWidgetInterface::StaticClass()))
		{
			INGFloatingWidgetInterface::Execute_UpdateMP(FloatingWidget, Percent);
		}
	}else
	{
		UE_LOG(LogTemp, Error, TEXT("Can't update MPBar"));
	}
}

bool ANGPawnBase::CanAddUnitOnCombatGrid(EGridType NewGridType) const
{
	UNGAbilitySystemComponent* ASC = CurrentGridAddress.GridOwnerPS ? CurrentGridAddress.GridOwnerPS->GetNGAbilitySystemComponent() : nullptr;
	if (const UNGPlayerAttributeSet* MyPlayerAttributeSet = ASC ? ASC->GetSet<UNGPlayerAttributeSet>() : nullptr)
	{
		if (CurrentGridAddress.GridType == EGridType::Wait && NewGridType == EGridType::Combat)
		{
			TArray<ANGPawnBase*> PlacedUnitPocket;
			CurrentGridAddress.GridOwnerPS->GetPlayerPocket()->GetPlacedUnits(PlacedUnitPocket);

			if (PlacedUnitPocket.Num() >= MyPlayerAttributeSet->GetLevel())
			{
				return false;
			}
		}
	}
	
	return true;
}

void ANGPawnBase::TryMoveTo(const FVector& TargetLocation)
{
	// Client에서만 불려야함
	if (HasAuthority())	return;
	
	//Client에서 미리 움직이고 서버에서 못간다 판단하면 reject
	EGridType NewGridType = UGridMapHelper::GetGridType(TargetLocation, CurrentGridAddress.GridOwnerPS);
	FIntVector2 NewIndex = UGridMapHelper::GetCellIndex(NewGridType, TargetLocation, CurrentGridAddress.GridOwnerPS);
	FGridAddress NewGridAddress(NewIndex, NewGridType, CurrentGridAddress.GridOwnerPS, CurrentGridAddress.DirtyFlag);
	
	if (!CanAddUnitOnCombatGrid(NewGridAddress.GridType))
	{
		UE_LOG(LogTemp, Warning, TEXT("Full of CombatGrid"));
		return;
	}
	
	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(NewGridAddress))
	{
		// UE_LOG(LogTemp, Log, TEXT("GridMap : %s %p, Index: %s"), NewGridType == EGridType::Combat ? TEXT("Combat") : TEXT("Wait"), GridMap, *NewIndex.ToString())
		if (!CanPlaceUnit(*GridMap, NewIndex))
		{
			return;
		}
		UpdateCurrentGridAddress(NewGridAddress);

		Server_TryMoveGrid(TargetLocation, NewGridAddress);
	}
}

void ANGPawnBase::UpdateCurrentGridAddress(FGridAddress NewGridAddress)
{
	PreGridAddress = CurrentGridAddress;
	
	CurrentGridAddress = NewGridAddress;
	++CurrentGridAddress.DirtyFlag;
	
	SetActorLocation(UGridMapHelper::GetWorldLocation(NewGridAddress));
}

void ANGPawnBase::UpdatePawnCurrentLocation(const FGridAddress& GridAddress)
{
	FVector Location = UGridMapHelper::GetWorldLocation(GridAddress);
	SetActorLocation(Location);
}

void ANGPawnBase::Client_RejectMove_Implementation()
{		
	CurrentGridAddress = PreGridAddress;
	++CurrentGridAddress.DirtyFlag;
	
	UpdatePawnCurrentLocation(CurrentGridAddress);
}

void ANGPawnBase::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();
	
	GrantHoverState();
}

void ANGPawnBase::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	
	RemoveHoverState();
}

void ANGPawnBase::GrantHoverState()
{
	if (bIsDrag)	return;
	
	//폰 하이라이트 -> 롤체는 파란색 아웃라이너
	if (ANGPlayerController* PC = GetOwner<ANGPlayerController>())
	{
		PC->SetHoveringUnit(this);
	}
	
	if (const UNGDeveloperSettings* Settings = GetDefault<UNGDeveloperSettings>())
	{
		if (!Settings->HoverOverlayMaterial.IsNull())
		{
			UMaterialInterface* HoverOverlayMaterial = Settings->HoverOverlayMaterial.LoadSynchronous();
   
			if (HoverOverlayMaterial && UnitMesh)
			{
				UnitMesh->SetOverlayMaterial(HoverOverlayMaterial);
			}
		}
	}
}

void ANGPawnBase::RemoveHoverState() const
{
	//폰 하이라이트 해제
	if (ANGPlayerController* PC = GetOwner<ANGPlayerController>())
	{
		PC->ClearHoveringUnit();
	}
	
	if (UnitMesh)
	{
		UnitMesh->SetOverlayMaterial(nullptr);
	}
}

bool ANGPawnBase::CanPlaceUnit(FGridMapBase& GridMap, FIntVector2 GridIndex)
{
	return GridMap.IsGridIndexEmpty(GridIndex) && GridMap.IsValidIndex(GridIndex);
}

void ANGPawnBase::Server_TryMoveGrid_Implementation(const FVector& TargetLocation, FGridAddress GridAddress)
{
	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(GridAddress))
	{
		if (ANGPlayerState* PS = GridAddress.GridOwnerPS)
		{
			if (UNGPocketComponent* Pocket = PS->GetPlayerPocket())
			{
				Pocket->TryMergeUnit(GetIdentificationTag());
			}
		}
		
		FIntVector2 NewIndex = UGridMapHelper::GetCellIndex(GridAddress.GridType, TargetLocation, GridAddress.GridOwnerPS);
		
		if (!CanPlaceUnit(*GridMap, NewIndex))
		{
			//클라이언트한테 언도 지시
			Client_RejectMove();
			return;
		}
		
		TranslatePawnOnGrid(GridAddress);
	}
}

void ANGPawnBase::TranslatePawnOnGrid(const FGridAddress& GridAddress)
{
	UnSetPawnOnGrid(CurrentGridAddress);
	SetPawnOnGrid(GridAddress);
}

void ANGPawnBase::SetPawnOnGrid(const FGridAddress& GridAddress)
{
	if (!HasAuthority())	return;
	
	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(GridAddress))
	{
		FGridData GridData;
		GridData.PlacedPawn = this;

		UpdateCurrentGridAddress(GridAddress);
		GridMap->SetGridData(GridAddress.GridIndex, GridData);
	}
}

void ANGPawnBase::UnSetPawnOnGrid(const FGridAddress& GridAddress) const
{
	if (!HasAuthority())	return;
	
	if (FGridMapBase* GridMap = UGridMapHelper::GetGridMap(GridAddress))
	{
		GridMap->EmptyGridMap(CurrentGridAddress.GridIndex);
	}
}

void ANGPawnBase::CheckAttackCondition()
{
	//여기서 DetectedTarget에 적이 있으면 ExecuteAttack, 없으면 FindingPath 및 길따라가기
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Attack"))))
	{
		return;
	}
	
	ExecuteAttack();
}

void ANGPawnBase::OnRep_CurrentGridAddress()
{
	// UE_LOG(LogTemp, Log, TEXT("OnRep_CurrentGridAddress %p"), this);
	UpdatePawnCurrentLocation(CurrentGridAddress);
}

void ANGPawnBase::OnRep_AnimationSet()
{
	ApplyAnimationSet();
}

void ANGPawnBase::InitializeUnitData(const FUnitData* Data)
{
	if (!HasAuthority())	return;
	
	if (!Data)	return;
	
	SetIdentificationTag(Data->IdentificationTag);
	
	if (Data->AnimationSet)
	{
		AnimationSet = Data->AnimationSet;
		
		ApplyAnimationSet();
	}
	
	UE_LOG(LogTemp, Log, TEXT("Success InitializeUnitData!"));
}

void ANGPawnBase::ApplyAnimationSet() const
{
	if (!AnimationSet)	return;
	
	if (USkeletalMeshComponent* Mesh = GetMesh())
	{
		Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		
		if (!AnimationSet->SkeletalMesh.IsNull())
		{
			// 상점 로딩 같은 대량 작업 시에는 Async 비동기 로드가 정석
			USkeletalMesh* LoadedMesh = AnimationSet->SkeletalMesh.LoadSynchronous();
			if (LoadedMesh && Mesh)
			{
				Mesh->SetSkeletalMesh(LoadedMesh);
			}
		}

		if (AnimationSet->AnimBlueprintClass && Mesh)
		{
			Mesh->SetAnimInstanceClass(AnimationSet->AnimBlueprintClass);
		}
		
		Mesh->InitAnim(true);
	}
}

void ANGPawnBase::LookAt(const ANGPawnBase* Target)
{
	if (!Target) return;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();
	
	FVector Direction = TargetLocation - MyLocation;
    
	Direction.Z = 0.f; 

	if (!Direction.IsNearlyZero())
	{
		FRotator Rotation = Direction.ToOrientationRotator();
        
		SetActorRotation(Rotation);
	}
}

void ANGPawnBase::ExecuteAttack()
{
	if (CurrentTarget.Get() && GetAbilitySystemComponent())
	{
		LookAt(CurrentTarget.Get());
		
		// UE_LOG(LogTemp, Log, TEXT("ExecuteAttack:: MyName: %s, Targetname: %s"), *GetName(), *CurrentTarget->GetName());
		
		FGameplayEventData Payload;
		Payload.Instigator = this;
		Payload.Target = CurrentTarget;
		Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(CurrentTarget);
		
		GetAbilitySystemComponent()->HandleGameplayEvent(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")), &Payload);
	}
}

void ANGPawnBase::VisualizePath() const
{
	if (PathFindingComponent)
	{
		TArray<FIntVector2> Path;
		PathFindingComponent->FindPathToClosestEnemy(CurrentGridAddress, OwnerIndex, Path);
		for (FIntVector2 PathIndex : Path)
		{
			FGridAddress NodeAddress(PathIndex, CurrentGridAddress.GridType, CurrentGridAddress.GridOwnerPS, 0);
			FVector WorldLocation = UGridMapHelper::GetWorldLocation(NodeAddress) + FVector(0.f, 0.f, 50.f);
			DrawDebugSphere(
				GetWorld(),             // 월드 컨텍스트
				WorldLocation,          // 구의 중심 좌표
				20.0f,                  // 반지름 (크기)
				12,                     // 세그먼트 수 (구의 형태, 너무 높으면 프레임 드랍 발생)
				FColor::Green,          // 색상 (경로는 초록색 추천)
				false,                  // bPersistentLines: false로 해야 계속 안 쌓입니다.
				-1.0f,                  // LifeTime: -1.0f로 두면 딱 다음 틱(1프레임)까지만 유지됩니다.
				0,                      // DepthPriority: 0은 기본 렌더링 순서
				2.0f                    // 선 두께
			);
		}
	}
}

void ANGPawnBase::HighlightRangeIndicator(const FGridAddress& PivotAddress) const
{
	//대기석은 ㄴㄴ
	if (PivotAddress.GridType != EGridType::Combat)
	{
		return;
	}
	
	if (AArena* CombatArena = PivotAddress.GridOwnerPS->GetHomeArena())
	{
		int32 HighlightRange = AttributeSet ? AttributeSet->GetAttackRange() : 1;
		CombatArena->HighlightAttackRange(PivotAddress, HighlightRange);
	}
}

void ANGPawnBase::BindJobSkillTrigger()
{
	if (!AbilitySystemComponent)	return;
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UNGPawnAttributeSet::GetManaAttribute())
	.AddLambda([this](const FOnAttributeChangeData& Data)
	{
		float CurrentMana = Data.NewValue;
		float MaxMana = AbilitySystemComponent->GetNumericAttribute(UNGPawnAttributeSet::GetMaxManaAttribute());

		if (CurrentMana >= MaxMana && MaxMana > 0.f)
		{
		   if (CurrentTarget)
		   {
			  LookAt(CurrentTarget.Get());

			  FGameplayEventData Payload;
			  Payload.Instigator = this;
			  Payload.Target = CurrentTarget.Get();
			  Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(CurrentTarget.Get());

			  GetAbilitySystemComponent()->HandleGameplayEvent(
				 FGameplayTag::RequestGameplayTag(FName("Event.JobSkill")), 
				 &Payload
			  );
		   }
		}
	});
}

bool ANGPawnBase::EquipItem(UNGEquipmentItemInstance* Item)
{
	if (EquipmentItems.Num() >= EquipMaxCount)	return false;
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(Item->GetEquipDataAsset()->EquipGameplayEffect,1,Context);

	if (!Spec.IsValid())	return false;
	
	EquipmentItems.Add(Item);

	Item->AppliedHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	
	return true;
}

TArray<TObjectPtr<UNGEquipmentItemInstance>> ANGPawnBase::UnEquipItem()
{
	for (UNGEquipmentItemInstance* Item : EquipmentItems)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(Item->AppliedHandle);
	}
	
	return MoveTemp(EquipmentItems);
}

void ANGPawnBase::InitAbilityData(const FUnitAbilityData& AbilityData)
{
	if (!HasAuthority())	return;
	if (!AbilitySystemComponent || !AttributeSet) return;
	
	if (ActiveLooseTags.Num() > 0)
	{
		AbilitySystemComponent->RemoveLooseGameplayTags(ActiveLooseTags);
	}
	
	// Setup Tag
	IdentificationTag = AbilityData.IdentificationTag;
	AbilitySystemComponent->AddLooseGameplayTags(AbilityData.OwnedTags);
	ActiveLooseTags = AbilityData.OwnedTags;
	
	if (AbilityData.JobSkill)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityData.JobSkill, 1);
		JobSkillAbilityHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
		BindJobSkillTrigger();
	}
	
	if (AbilityData.DefaultAttackSkill)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityData.DefaultAttackSkill, 1);
		AttackAbilitySpecHandle = AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(AbilityData.DefaultAttackSkill, 1, INDEX_NONE, this)
		);
	}
	
	// Initialize Stat
	for (const auto& Stat : AbilityData.DefaultStats)
	{
		AbilitySystemComponent->SetNumericAttributeBase(Stat.Key, Stat.Value);
	}
}
