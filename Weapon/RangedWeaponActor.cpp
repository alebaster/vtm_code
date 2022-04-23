#include "RangedWeaponActor.h"

#include "Inventory/InventoryComponent.h"
#include "Weapon/WeaponFx.h"

#include "Character/WodComponent.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ARangedWeaponActor::ARangedWeaponActor()
{
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh"));
	RootComponent = mesh;

	//PrimaryComponentTick.bCanEverTick = false;
}

void ARangedWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	if (weapon_fx_bp)
		weapon_fx = NewObject<UWeaponFx>(this, "", EObjectFlags::RF_NoFlags, weapon_fx_bp->GetDefaultObject());
	if (!weapon_fx)
		UE_LOG(LogTemp, Error, TEXT("weapon_fx is NULL"));
}

void ARangedWeaponActor::Fire()
{
	if (!item_descr)
	{
		UE_LOG(LogTemp, Error, TEXT("item_descr of current weapon is null"));
		return;
	}

	if (current_clip == 0)
		return;

	current_clip--;
	//ammo--; // after reloading
	if (infinit_ammo)
		ammo = 999;

	//UE_LOG(LogTemp, Error, TEXT("current_clip: %d"), current_clip);
	//UE_LOG(LogTemp, Error, TEXT("ammo: %d"), ammo);

	int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	float CurrentSpread = (float)item_descr->spread;
	float SpreadCone = FMath::DegreesToRadians(CurrentSpread);

	FVector CameraLoc;
	FRotator CameraRot;
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//AAIController* aiController = Cast<AsbrAIController>(OwnerPawn->Controller);
	if (playerController)
		playerController->GetPlayerViewPoint(CameraLoc, CameraRot);
	//if (aiController)
	//{
	//	CameraRot = aiController->GetControlRotation();
	//	CameraLoc = WeaponMesh->GetSocketLocation("FireSocket");
	//}

	FVector AimDirection = CameraRot.Vector();
	FVector StartAim = CameraLoc;
	FTransform StartTransform = mesh->GetSocketTransform("FireSocket");
	//FVector ShootDirection = WeaponRandomStream.VRandCone(AimDirection, SpreadCone, SpreadCone);
	FVector ShootDirection = AimDirection;
	FVector EndTrace = StartAim + ShootDirection * 10 * item_descr->range;

	FHitResult Impact(ForceInit);

	FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.AddIgnoredActor(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));

	GetWorld()->LineTraceSingleByChannel(Impact, StartAim, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);

	//ProcessInstantHit(Impact, StartTrace, ShootDirection, RandomSeed, CurrentSpread);

	//FVector EndTrace = StartTrace + ShootDirection * current_weapon->item_descr->range;
	FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

	//DrawDebugLine(this->GetWorld(), StartAim, EndPoint, FColor::Red, true, 10000, 10.0f);

	//if (Impact.GetActor())
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Impact.GetActor(): %s"), *GetOwner()->GetName());
	//	UE_LOG(LogTemp, Error, TEXT("Impact.GetActor(): %s"), *Impact.GetActor()->GetName());
	//}
	//UE_LOG(LogTemp, Error, TEXT("StartAim: %s"), *StartAim.ToString());
	//UE_LOG(LogTemp, Error, TEXT("EndPoint: %s"), *EndPoint.ToString());

	if (Impact.bBlockingHit)
	{
		AActor* actor = Impact.GetActor();
		if (actor == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("static mesh impact"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("actor impact"));

			//TArray<UActorComponent*> all_components;
			//actor->GetComponents(all_components);
			//for (auto c : all_components)
			//{
			//	UWodComponent* wod_component = Cast<UWodComponent>(c);
			//	if (wod_component)
			//	{
			//		wod_component->ChangeHP(-30); // item_descr->
			//	}
			//		
			//}
			const FDamageEvent de;
			actor->TakeDamage(50, de, 0, 0);
		}
	}

	if (weapon_fx)
		weapon_fx->SpawnEffect(Impact, StartTransform, EndPoint);

	PlayWeaponSound(weapon_sound.fire);
}

void ARangedWeaponActor::Fire(FVector target)
{
	if (!item_descr)
	{
		UE_LOG(LogTemp, Error, TEXT("item_descr of current weapon is null"));
		return;
	}

	//if (current_clip == 0)
	//	return;

	current_clip--;
	if (infinit_ammo)
		ammo = 999;

	int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	float CurrentSpread = (float)item_descr->spread;
	float SpreadCone = FMath::DegreesToRadians(CurrentSpread);

	FTransform StartTransform = mesh->GetSocketTransform("FireSocket");
	target.Z += 15;
	FVector AimDirection = target - StartTransform.GetLocation();
	AimDirection.Normalize();
	FVector StartAim = StartTransform.GetLocation();
	//FVector ShootDirection = WeaponRandomStream.VRandCone(AimDirection, SpreadCone, SpreadCone);
	FVector ShootDirection = AimDirection;
	FVector EndTrace = StartAim + ShootDirection * 10 * item_descr->range;

	FHitResult Impact(ForceInit);

	FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(Impact, StartAim, EndTrace, ECollisionChannel::ECC_Pawn, TraceParams);

	//ProcessInstantHit(Impact, StartTrace, ShootDirection, RandomSeed, CurrentSpread);

	//FVector EndTrace = StartTrace + ShootDirection * current_weapon->item_descr->range;
	FVector EndPoint = Impact.GetActor() ? Impact.ImpactPoint : EndTrace;

	//DrawDebugLine(this->GetWorld(), StartAim, EndPoint, FColor::Red, true, 10000, 10.0f);

	if (Impact.bBlockingHit)
	{
		AActor* actor = Impact.GetActor();
		if (actor == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("static mesh impact"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("actor impact"));

			const FDamageEvent de;
			actor->TakeDamage(30, de, 0, 0);
		}
	}

	if (weapon_fx)
		weapon_fx->SpawnEffect(Impact, StartTransform, EndPoint);

	PlayWeaponSound(weapon_sound.fire);
}

void ARangedWeaponActor::Reload()
{
	if (!item_descr)
		return;

	if (ammo == 0)
		return;

	int need = item_descr->clip_size - current_clip;
	if (need == 0)
		return;

	int remaining = current_clip;
	current_clip = (ammo < need) ? (ammo + remaining) : item_descr->clip_size;
	ammo -= current_clip-remaining;

	PlayWeaponSound(weapon_sound.reload);
}

void ARangedWeaponActor::PlayWeaponSound(USoundWave* snd)
{
	if (!snd || !mesh)
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), snd, mesh->GetComponentLocation());
	MakeNoise(1.0f, UGameplayStatics::GetPlayerPawn(this, 0), mesh->GetComponentLocation());
}