#include "WeaponFx.h"

#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/PhysicalMaterials/PhysicalMaterial.h"

#include "BulletProjectile.h"

void UWeaponFx::SpawnEffect(const FHitResult& Impact, const FTransform& StartTransform, const FVector& EndPoint)
{
	//UE_LOG(LogTemp, Warning, TEXT("pew effect"));

	FVector StartPoint = StartTransform.GetLocation();

	SpawnImpactEffect(Impact);
	SpawnImpactDecal(Impact);
	SpawnTrailEffect(StartPoint, EndPoint);
	SpawnMuzzleEffect(StartTransform);
}

void UWeaponFx::SpawnImpactEffect(const FHitResult& Impact)
{
	if (!Impact.PhysMaterial.IsValid())
		return;

	//UE_LOG(LogTemp, Error, TEXT("PhysMaterial TYPE %d"), (int)Impact.PhysMaterial->SurfaceType);

	int index = 0;
	USoundWave* sound = 0;
	UParticleSystem* effect = 0;
	switch (Impact.PhysMaterial->SurfaceType)
	{
	case EPhysicalSurface::SurfaceType1:// carpet
		effect = 0;
		sound = 0;
		break;
	case EPhysicalSurface::SurfaceType2:// concrete
		effect = particles.concrete;
		index = FMath::RandRange(0, sounds.ConcreteSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.ConcreteSounds[index];
		break;
	case EPhysicalSurface::SurfaceType3:// ground
		effect = particles.dirt;
		index = FMath::RandRange(0, sounds.DirtSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.DirtSounds[index];
		break;
	case EPhysicalSurface::SurfaceType4:// metal
		effect = particles.metal;
		index = FMath::RandRange(0, sounds.MetalSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.MetalSounds[index];
		break;
	case EPhysicalSurface::SurfaceType5:// sand
		effect = particles.sand;
		index = FMath::RandRange(0, sounds.SandSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.SandSounds[index];
		break;
	case EPhysicalSurface::SurfaceType6:// water
		effect = particles.water;
		index = FMath::RandRange(0, sounds.WaterSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.WaterSounds[index];
		break;
	case EPhysicalSurface::SurfaceType7:// wood
		effect = particles.wood;
		index = FMath::RandRange(0, sounds.WoodSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.WoodSounds[index];
		break;
	case EPhysicalSurface::SurfaceType8:// blood
		effect = particles.blood;
		index = FMath::RandRange(0, sounds.BloodSounds.Num()-1);
		sound = index < 0 ? 0 : sounds.BloodSounds[index];
		//UE_LOG(LogTemp, Error, TEXT("PhysMaterial is valid"));

		break;
	default:
		effect = 0;
		break;
	}

	if (effect)
	{
		if (effect == particles.blood)
			UGameplayStatics::SpawnEmitterAtLocation(this, effect, Impact.ImpactPoint, Impact.ImpactNormal.Rotation(), FVector(0.1f, 0.1f, 0.1f));
		else
			UGameplayStatics::SpawnEmitterAtLocation(this, effect, Impact.ImpactPoint, Impact.ImpactNormal.Rotation(), FVector(0.5f, 0.5f, 0.5f));
	}

	if (sound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, Impact.ImpactPoint, 2.0f);
}

void UWeaponFx::SpawnImpactDecal(const FHitResult& Impact)
{
	if (!Impact.PhysMaterial.IsValid())
		return;

	UMaterial* decal = 0;

	switch (Impact.PhysMaterial->SurfaceType)
	{
	case EPhysicalSurface::SurfaceType2:// concrete
		decal = concrete_decal;
		break;
	case EPhysicalSurface::SurfaceType4:// metal
		decal = metal_decal;
		break;
	case EPhysicalSurface::SurfaceType7:// wood
		decal = wood_decal;
		break;
	case EPhysicalSurface::SurfaceType8:// blood
		decal = blood_decal;
		break;
	default:
		decal = concrete_decal;
		break;
	}

	if (!decal)
		return;

	if (decal == blood_decal)
	{
		UGameplayStatics::SpawnDecalAttached(wood_decal, FVector(3.0f, 3.0f, 3.0f), Impact.GetComponent(), FName("none"), Impact.ImpactPoint, Impact.ImpactNormal.Rotation(), EAttachLocation::Type::SnapToTarget, 120.0f);
		//if (Impact.GetActor())
		//{
		//	if (Impact.GetActor()->IsRootComponentMovable()) 
		//	{
		//		USkeletalMeshComponent* MeshRootComp = Cast<USkeletalMeshComponent>(Impact.GetActor()->GetRootComponent());
		//
		//		UGameplayStatics::SpawnDecalAttached(wood_decal, FVector(3.0f, 3.0f, 3.0f), MeshRootComp, FName("Bip01"), Impact.ImpactPoint, Impact.ImpactNormal.Rotation(), EAttachLocation::Type::KeepRelativeOffset, 120.0f);
		//	}
		//}
	}
	else
		UGameplayStatics::SpawnDecalAtLocation(this, wood_decal, FVector(3.0f, 3.0f, 3.0f), Impact.ImpactPoint, Impact.ImpactNormal.Rotation(), 120.0f);
}

void UWeaponFx::SpawnTrailEffect(const FVector& StartPoint, const FVector& EndPoint)
{
	if (!projectile_bp)
	{
		UE_LOG(LogTemp, Error, TEXT("projectile_bp is NULL"));
		return;
	}
	
	FVector dir = EndPoint - StartPoint;
	dir.Normalize();
	FRotator rot = dir.Rotation();

	ABulletProjectile* projectile = Cast<ABulletProjectile>(GetWorld()->SpawnActor(projectile_bp->GetDefaultObject()->GetClass(), &StartPoint, &rot));
	if (projectile)
		projectile->SetVelocity(dir,bullet_projectile_speed);
	else
		UE_LOG(LogTemp, Error, TEXT("projectile is NULL"));
}

void UWeaponFx::SpawnMuzzleEffect(const FTransform& StartTransform)
{
	if (!muzzle_flash)
		return;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzle_flash, StartTransform);
}