#include "BulletProjectile.h"

ABulletProjectile::ABulletProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(0.5f);
	RootComponent = CollisionComponent;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->OnProjectileStop.AddDynamic(this, &ABulletProjectile::OnImpact);
	//ProjectileMovement->InitialSpeed = 3000.f;
	//ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	//ProjectileMovement->Bounciness = 0.3f;
}

void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletProjectile::SetVelocity(FVector direction, float v)
{
	if (ProjectileMovement)
		ProjectileMovement->Velocity = direction * v;
}

void ABulletProjectile::OnImpact(const FHitResult& HitResult)
{
	ProjectileMovement->StopMovementImmediately();
	//SetLifeSpan(2.0f);
	Destroy();
}