// Copyright (c) 2025 TeamNG. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "Interface/Poolable.h"
#include "NGProjectile.generated.h"

class ANGPawnBase;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class PROJECTNG_API ANGProjectile : public AActor, public IPoolable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANGProjectile();

	virtual void Tick(float DeltaTime) override;
	
	virtual void Activate() override;
	virtual void Deactivate() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	UFUNCTION()
	void ReleaseProjectile();
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;
	
	//어빌리티로부터 전달받은 데미지 정보
	FGameplayEffectSpecHandle SpecHandle;	
	
	UPROPERTY(VisibleAnywhere)
	ANGPawnBase* Target;
	
	UPROPERTY(BlueprintReadWrite, Category = "Setting | Projectile")
	float MoveSpeed = 2000.f;

	UPROPERTY(BlueprintReadWrite, Category = "Setting | Projectile")
	float RotateSpeed = 10.f;
	
public:
	void SetSpecHandle(const FGameplayEffectSpecHandle InSpecHandle) { SpecHandle = InSpecHandle; }
	
	void SetTarget(ANGPawnBase* NewTarget);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;
	
};
