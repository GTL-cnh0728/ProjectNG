// Copyright (c) 2025 TeamNG. All Rights Reserved.


#include "Core/NGPoolSubSystem.h"

#include "Combat/NGProjectile.h"

ANGProjectile* UNGPoolSubSystem::AcquireProjectile(TSubclassOf<ANGProjectile> ProjectileClass,
                                                   const FTransform& SpawnTransform, ANGPawnBase* Target)
{
	if (!ProjectileClass)	return nullptr;
	
	FNGProjectileList& Pool = ProjectilePools.FindOrAdd(ProjectileClass);
	ANGProjectile* Projectile;
	bool bNewSpawnProjectile = false;
	
	//여유분이 있으면 반환
	if (Pool.FreeProjectileList.Num() > 0)
	{
		Projectile = Pool.FreeProjectileList.Pop();
	}
	else
	{
		Projectile = GetWorld()->SpawnActorDeferred<ANGProjectile>(ProjectileClass, SpawnTransform);
		bNewSpawnProjectile = true;
	}
	
	if (Projectile)
	{
		Projectile->SetActorTransform(SpawnTransform);
		
		Projectile->SetActorHiddenInGame(false);
		Projectile->SetActorEnableCollision(true);
		Projectile->SetActorTickEnabled(true);
		
		Projectile->SetTarget(Target);
		
		if (!Projectile->HasActorBegunPlay() && bNewSpawnProjectile)
		{
			Projectile->FinishSpawning(SpawnTransform);
		}
	}
	
	return Projectile;
}


ANGPawnBase* UNGPoolSubSystem::AcquirePawn(TSubclassOf<ANGPawnBase> PawnClass,
                                           const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnParams)
{
	if (!PawnClass)	return nullptr;
	
	FNGPawnList& Pool = PawnPools.FindOrAdd(PawnClass);
	
	ANGPawnBase* Pawn;
	
	//여유분이 있으면 반환
	if (Pool.FreePawnList.Num() > 0)
	{
		Pawn = Pool.FreePawnList.Pop();
		Pawn->SetActorTransform(SpawnTransform);
	}
	else
	{
		UClass* TargetClass = PawnClass;
		
		Pawn = GetWorld()->SpawnActor<ANGPawnBase>(TargetClass, SpawnTransform, SpawnParams);
		UE_LOG(LogTemp, Log, TEXT("Spawn Default Pawn Class"));
	}
	return Pawn;
}

void UNGPoolSubSystem::ReleaseDefaultSetting(AActor* InActor)
{
	check(InActor);
	
	InActor->SetActorHiddenInGame(true);
	InActor->SetActorEnableCollision(false);
	InActor->SetActorTickEnabled(false);
}

void UNGPoolSubSystem::ReleaseSegment(ANGPawnBase* Pawn)
{
	if (!Pawn)	return;
	
	Pawn->Deactivate();
	
	FNGPawnList& Pool = PawnPools.FindOrAdd(Pawn->GetClass());
	Pool.FreePawnList.Push(Pawn);
}

void UNGPoolSubSystem::ReleaseSegment(ANGProjectile* Projectile)
{
	if (!Projectile)	return;
	
	ReleaseDefaultSetting(Projectile);
	
	FNGProjectileList& Pool = ProjectilePools.FindOrAdd(Projectile->GetClass());
	Pool.FreeProjectileList.Push(Projectile);
}