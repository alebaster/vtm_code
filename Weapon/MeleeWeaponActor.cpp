#include "MeleeWeaponActor.h"

#include "Inventory/InventoryComponent.h"
//#include "Weapon/WeaponFx.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

AMeleeWeaponAnimation::AMeleeWeaponAnimation()
{
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh"));
	RootComponent = mesh;

	//PrimaryComponentTick.bCanEverTick = false;
}

void AMeleeWeaponAnimation::BeginPlay()
{
	Super::BeginPlay();

	//if (weapon_fx_bp)
	//	weapon_fx = NewObject<UWeaponFx>(this, "", EObjectFlags::RF_NoFlags, weapon_fx_bp->GetDefaultObject());
	//if (!weapon_fx)
	//	UE_LOG(LogTemp, Error, TEXT("weapon_fx is NULL"));
}

void AMeleeWeaponAnimation::Attack()
{

}