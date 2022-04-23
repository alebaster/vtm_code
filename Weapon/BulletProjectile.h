#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/GameFramework/ProjectileMovementComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"


#include "BulletProjectile.generated.h"

UCLASS()
class VTM_API ABulletProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletProjectile();

	void SetVelocity(FVector direction, float v);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement=0;

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* CollisionComponent=0;

	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);
};
