#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystem.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Materials/Material.h"

#include "WeaponFx.generated.h"

class ABulletProjectile;

USTRUCT(BlueprintType)
struct FImpactParticles
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* blood;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* wood;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* metal;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* concrete;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* water;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* dirt;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* sand;
};

USTRUCT(BlueprintType)
struct FImpactSounds
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> BloodSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> WoodSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> MetalSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> ConcreteSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> WaterSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> DirtSounds;

	UPROPERTY(EditDefaultsOnly)
	TArray<USoundWave*> SandSounds;
};

UCLASS(Blueprintable)
class VTM_API UWeaponFx : public UObject
{
	GENERATED_BODY()
	
public:
	void SpawnEffect(const FHitResult& Impact, const FTransform& StartTransform,const FVector& EndPoint);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABulletProjectile> projectile_bp=0;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* wood_decal=0;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* metal_decal = 0;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* concrete_decal = 0;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* blood_decal = 0;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* muzzle_flash=0;

	UPROPERTY(EditDefaultsOnly)
	FImpactParticles particles;

	UPROPERTY(EditDefaultsOnly)
	FImpactSounds sounds;

	UPROPERTY(EditDefaultsOnly)
	float bullet_projectile_speed = 10.0f;

private:
	void SpawnImpactEffect(const FHitResult& Impact);
	void SpawnImpactDecal(const FHitResult& Impact);
	void SpawnTrailEffect(const FVector& StartPoint, const FVector& EndPoint);
	void SpawnMuzzleEffect(const FTransform& StartTransform);
};
