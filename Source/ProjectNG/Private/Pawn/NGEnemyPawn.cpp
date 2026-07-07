// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Pawn/NGEnemyPawn.h"

#include "AbilitySystem/NGAbilitySystemComponent.h"
#include "AbilitySystem/NGPawnAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Game/NGPawnDataManager.h"
#include "Player/NGPlayerController.h"
#include "ProjectNG/ProjectNG.h"


// Sets default values
ANGEnemyPawn::ANGEnemyPawn()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
}

void ANGEnemyPawn::Activate()
{
	Super::Activate();
	
	if (HasAuthority())
	{
		Multicast_Activate();
	
		if (UNGPawnDataManager* PawnDataManager = GetWorld()->GetGameInstance()->GetSubsystem<UNGPawnDataManager>())
		{
			if (const FUnitAbilityData* EnemyData = PawnDataManager->GetUnitAbilityData(IdentificationTag))
			{
				InitializeEnemyStats(*EnemyData);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[%s] 데이터 테이블에서 태그(%s)를 찾을 수 없습니다!"), *GetName(), *IdentificationTag.ToString());
			}
		}
	}
}

void ANGEnemyPawn::InitializeEnemyStats(const FUnitAbilityData& DataRow)
{
	if (!AbilitySystemComponent || !AttributeSet) return;

	if (HasAuthority())
	{
		if (ActiveLooseTags.Num() > 0)
		{
			AbilitySystemComponent->RemoveLooseGameplayTags(ActiveLooseTags);
		}
		
		// Setup Tag
		IdentificationTag = DataRow.IdentificationTag;
		
		// Initialize Stat
		for (const auto& Stat : DataRow.DefaultStats)
		{
			AbilitySystemComponent->SetNumericAttributeBase(Stat.Key, Stat.Value);
		}
	}
}

void ANGEnemyPawn::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	Activate();
}

// Called when the game starts or when spawned
void ANGEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
}

void ANGEnemyPawn::Die()
{
	ANGPlayerController* PC = GetOwner<ANGPlayerController>();
	if (ANGPlayerState* PS = PC ? PC->GetPlayerState<ANGPlayerState>() : nullptr)
	{
		PS->OnDieCPUEnemy();
	}

	Super::Die();
}

// Called every frame
void ANGEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


// Called to bind functionality to input
void ANGEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANGEnemyPawn::Initialize(ANGPlayerState* PS)
{
	Super::Initialize(PS);
	
	OwnerIndex = -1;
}

void ANGEnemyPawn::InitAbilityActorInfo()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}
