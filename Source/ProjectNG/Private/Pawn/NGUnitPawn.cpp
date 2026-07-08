// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Pawn/NGUnitPawn.h"

#include "AbilitySystem/NGAbilitySystemComponent.h"
#include "AbilitySystem/NGPawnAttributeSet.h"
#include "AbilitySystem/NGGameplayAbility.h"
#include "Player/NGPlayerController.h"
#include "ProjectNG/ProjectNG.h"

// Sets default values
ANGUnitPawn::ANGUnitPawn() : AcceptanceRadius(1.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	if (UnitMesh)
	{
		UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		UnitMesh->SetCollisionResponseToChannel(ECC_SelectableUnit, ECR_Block);
	}
}

void ANGUnitPawn::InitializeAttributes()
{
	Super::InitializeAttributes();
}

void ANGUnitPawn::OnSelected_Implementation()
{
	ISelectableInterface::OnSelected_Implementation();
	UE_LOG(LogTemp, Log, TEXT("OnSelected"));
	
	// RangeIndicator 만들거면 GridAddress 인자 받아와서 완성하기
	// ShowRangeIndicator(true, );
	
	bIsSelected = true;
}

void ANGUnitPawn::OnDeselected_Implementation()
{
	ISelectableInterface::OnDeselected_Implementation();
	// UE_LOG(LogTemp, Log, TEXT("OnDeselected"));
	
	bIsSelected = false;
}

void ANGUnitPawn::OnDrag_Implementation()
{
	ISelectableInterface::OnDrag_Implementation();

	// UE_LOG(LogTemp, Log, TEXT("OnDrag"));
	//잡힌 모션 -> 레그돌화 해도 ㄱㅊ을듯
	bIsDrag = true;
}

void ANGUnitPawn::OnUndrag_Implementation()
{
	ISelectableInterface::OnUndrag_Implementation();

	// UE_LOG(LogTemp, Log, TEXT("OnUndrag"));
	
	bIsDrag = false;
}

// Called when the game starts or when spawned
void ANGUnitPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ANGUnitPawn::Activate()
{
	Super::Activate();
	
	InitAbilityActorInfo();
	
	if (!AttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("ANGUnitCharacter::No attribute set"));
		return;
	}

}

void ANGUnitPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	Activate();
}

void ANGUnitPawn::Deactivate()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackCheckTimerHandle);
	
	Super::Deactivate();
}

void ANGUnitPawn::InitAbilityActorInfo()
{
	// ASC 복사 및 아바타 설정
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}


// Called to bind functionality to input
void ANGUnitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
