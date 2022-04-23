#include "HandsActor.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "TimerManager.h"

#include "Core/RuntimeUtils.h"
#include "VtmPlayerController.h"
#include "Inventory/InventoryComponent.h"

AHandsActor::AHandsActor()
{
	//PrimaryComponentTick.bCanEverTick = true;

	//capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule"));
	//RootComponent = capsule;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	RootComponent = HandsMesh;
	//HandsMesh->SetupAttachment(RootComponent);

	LockPickMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LockPickMesh"));
	LockPickMesh->SetupAttachment(HandsMesh);

	fsm = CreateDefaultSubobject<UStateMachineComponent>(TEXT("HandsFSM"));
}

void AHandsActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);

	if (fsm)
	{
		fsm->RegisterComponent();
		SetupFSM();
	}
}

//void AHandsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}

void AHandsActor::EquipLockpick()
{
    if (!HandsMesh || !LockPickMesh)
        return;
    
	if (!LockPickAnimation.equip || !LockPickAnimationHands.equip)
	{
		UE_LOG(LogTemp, Error, TEXT("no lockpick equip animations"));
		return;
	}
    
	//HandsMesh->SetHiddenInGame(false);
	//HandsMesh->SetVisibility(true,true);
	//LockPickMesh->SetHiddenInGame(false);
	SetActorHiddenInGame(false);

    isFree = false;
    float d = URuntimeUtils::PlayMontage(LockPickMesh, LockPickAnimation.equip);
	URuntimeUtils::PlayMontage(HandsMesh, LockPickAnimationHands.equip);
	
	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::LockPickEquipEnd, d, false);
}

void AHandsActor::LockPickEquipEnd()
{
    if (!LockPickAnimation.start || !LockPickAnimationHands.start)
        return;
	
    float d = URuntimeUtils::PlayMontage(LockPickMesh, LockPickAnimation.start);
    URuntimeUtils::PlayMontage(HandsMesh, LockPickAnimationHands.start);
    
    GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::PickLockPick, d, false);
}

void AHandsActor::PickLockPick()
{
    if (!LockPickAnimation.pick || !LockPickAnimationHands.pick)
        return;
    
    // LOOP
    URuntimeUtils::PlayMontage(LockPickMesh, LockPickAnimation.pick, true);
    URuntimeUtils::PlayMontage(HandsMesh, LockPickAnimationHands.pick, true);
}

void AHandsActor::EndLockpick()
{
    if (!LockPickAnimation.end || !LockPickAnimationHands.end)
        return;
    
    float d = URuntimeUtils::PlayMontage(LockPickMesh, LockPickAnimation.end);
    URuntimeUtils::PlayMontage(HandsMesh, LockPickAnimationHands.end);

	//HandsMesh->SetHiddenInGame(true);
	//LockPickMesh->SetHiddenInGame(true);
    
    GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::EndLockPickAnimation, d, false);
}

void AHandsActor::EndLockPickAnimation()
{
	//HandsMesh->SetHiddenInGame(true);
	//LockPickMesh->SetHiddenInGame(true);

	SetActorHiddenInGame(true);

	FreeHands();

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
}

void AHandsActor::EquipRangedWeapon(ARangedWeaponActor* weapon)
{
	if (!weapon)
		UE_LOG(LogTemp, Error, TEXT("weapon mesh is null"));

	if (!HandsMesh)
		UE_LOG(LogTemp, Error, TEXT("hands mesh is null"));

	UE_LOG(LogTemp, Warning, TEXT("equip weapon with id: %s"), *weapon->id);

	if (current_weapon == weapon)
	{
		equip_new = false;
		return;
	}
	else if (current_weapon != weapon && current_weapon!=0)
		equip_new = true;
	else
		equip_new = false;

	WeaponMesh = weapon->mesh;
	current_weapon = weapon;

	equip_pressed = true;
}

void AHandsActor::FreeHands()
{
    isFree = true;
}

void AHandsActor::PlayWeaponSound(USoundWave* snd)
{
	if (!snd || !WeaponMesh)
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), snd, WeaponMesh->GetComponentLocation());
	MakeNoise(1.0f, UGameplayStatics::GetPlayerPawn(this,0), WeaponMesh->GetComponentLocation());
}

// fsm
void AHandsActor::SetupFSM()
{
	fsm->AddState("hidden", &H_enter_delegate,0, &H_exit_delegate);
	fsm->AddState("equipping", &E_enter_delegate, 0, &E_exit_delegate);
	fsm->AddState("idle", &I_enter_delegate, 0, &I_exit_delegate);
	fsm->AddState("firing", &F_enter_delegate, 0, &F_exit_delegate);
	fsm->AddState("realoading", &R_enter_delegate, 0, &R_exit_delegate);

	H_enter_delegate.BindUObject(this, &AHandsActor::H_enter_func);
	H_exit_delegate.BindUObject(this, &AHandsActor::H_exit_func);

	E_enter_delegate.BindUObject(this, &AHandsActor::E_enter_func);
	E_exit_delegate.BindUObject(this, &AHandsActor::E_exit_func);

	I_enter_delegate.BindUObject(this, &AHandsActor::I_enter_func);
	I_exit_delegate.BindUObject(this, &AHandsActor::I_exit_func);

	F_enter_delegate.BindUObject(this, &AHandsActor::F_enter_func);
	F_exit_delegate.BindUObject(this, &AHandsActor::F_exit_func);

	R_enter_delegate.BindUObject(this, &AHandsActor::R_enter_func);
	R_exit_delegate.BindUObject(this, &AHandsActor::R_exit_func);

	fsm->AddTransition("hidden", "equipping", false, &HtoE_rule_delegate);
	fsm->AddTransition("equipping", "idle", false, &EtoI_rule_delegate);
	fsm->AddTransition("idle", "hidden", false, &ItoH_rule_delegate);

	fsm->AddTransition("idle", "firing", false, &ItoF_rule_delegate);
	fsm->AddTransition("firing", "idle", true, &FtoI_rule_delegate);

	fsm->AddTransition("idle", "realoading", false, &ItoR_rule_delegate);
	fsm->AddTransition("realoading", "idle", true, &RtoI_rule_delegate);

	fsm->AddTransition("idle", "equipped", false, &ItoE_rule_delegate);

	HtoE_rule_delegate.BindUObject(this, &AHandsActor::HtoE_rule);
	ItoH_rule_delegate.BindUObject(this, &AHandsActor::ItoH_rule);

	ItoF_rule_delegate.BindUObject(this, &AHandsActor::ItoF_rule);
	FtoI_rule_delegate.BindUObject(this, &AHandsActor::FtoI_rule);

	ItoR_rule_delegate.BindUObject(this, &AHandsActor::ItoR_rule);
	RtoI_rule_delegate.BindUObject(this, &AHandsActor::RtoI_rule);

	EtoI_rule_delegate.BindUObject(this, &AHandsActor::EtoI_rule);
	ItoE_rule_delegate.BindUObject(this, &AHandsActor::ItoE_rule);

	fsm->SetDefaultState("hidden");
}

bool AHandsActor::HtoE_rule()
{
	return (equip_pressed&&isFree);
}

bool AHandsActor::ItoH_rule()
{
	return (hide_pressed&&isFree);
}

bool AHandsActor::ItoF_rule()
{
	return (fire_pressed&&isFree);
}

bool AHandsActor::FtoI_rule()
{
	return (isFree);
}

bool AHandsActor::ItoR_rule()
{
	return (reload_pressed&&isFree);
}

bool AHandsActor::RtoI_rule()
{
	return (isFree);
}

bool AHandsActor::EtoI_rule()
{
	return (isFree);
}

bool AHandsActor::ItoE_rule()
{
	return (equip_pressed&&equip_new&&isFree);
}

void AHandsActor::H_enter_func()
{
	isFree = false;

	if (equip_new)
		equip_pressed = true;

	if (!current_weapon->weapon_anim.hide || !current_weapon->hands_anim.hide)
	{
		UE_LOG(LogTemp, Error, TEXT("no hide animations"));
		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedHide_animationEnd, 1, false);

		return;
	}

	float d = URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.hide);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.hide);

	PlayWeaponSound(current_weapon->weapon_sound.hide);

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedHide_animationEnd, d, false);
}

void AHandsActor::RangedHide_animationEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);

	//HandsMesh->SetHiddenInGame(true);
	//WeaponMesh->SetHiddenInGame(true);
	SetActorHiddenInGame(true);

	FreeHands();
	current_weapon = 0;
}

void AHandsActor::H_exit_func()
{
	hide_pressed = false;
}

void AHandsActor::E_enter_func()
{
	if (equip_new)
	{
		FreeHands();
		hide_pressed = true;
		return;
	}

	isFree = false;

	if (!current_weapon->weapon_anim.equip || !current_weapon->hands_anim.equip)
	{
		UE_LOG(LogTemp, Error, TEXT("no equip animations"));
		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedEquip_animationEnd, 1, false);

		return;
	}

	//HandsMesh->SetHiddenInGame(false);
	//WeaponMesh->SetHiddenInGame(false);
	SetActorHiddenInGame(false);

	float d = URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.equip);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.equip);

	PlayWeaponSound(current_weapon->weapon_sound.equip);

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedEquip_animationEnd, d, false);
}

void AHandsActor::RangedEquip_animationEnd()
{
	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);

	if (!current_weapon->weapon_anim.idle || !current_weapon->hands_anim.idle)
	{
		UE_LOG(LogTemp, Error, TEXT("no idle animations"));
		FreeHands();

		return;
	}

	URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.idle,true);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.idle,true);

	FreeHands();
}

void AHandsActor::E_exit_func()
{
	equip_pressed = false;

	// stop animation?
}

void AHandsActor::I_enter_func()
{
	isFree = false;

	if (!current_weapon->weapon_anim.idle || !current_weapon->hands_anim.idle)
	{
		UE_LOG(LogTemp, Error, TEXT("no idle animations"));
		return;
	}

	//HandsMesh->SetHiddenInGame(false);
	//WeaponMesh->SetHiddenInGame(false);
	SetActorHiddenInGame(false);

	float d = URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.idle,true);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.idle,true);

	//GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	//GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedEquip_animationEnd, d, false);

	FreeHands();
}

void AHandsActor::I_exit_func()
{

}

void AHandsActor::F_enter_func()
{
	isFree = false;

	if (!current_weapon)
		return;

	if (current_weapon->current_clip == 0)
	{
		PlayWeaponSound(current_weapon->weapon_sound.empty);
		float d = 0.0f;
		if (!current_weapon->weapon_sound.empty)
			UE_LOG(LogTemp, Error, TEXT("no sound: empty"))
		else
			d = current_weapon->weapon_sound.empty->Duration;

		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedFire_animationEnd, d, false);

		return;
	}

	if (!current_weapon->weapon_anim.fire || !current_weapon->hands_anim.fire)
	{
		UE_LOG(LogTemp, Error, TEXT("no animations: fire"))
		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedFire_animationEnd, 1, false);
		FireRangedWeapon();

		return;
	}

	float d = URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.fire);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.fire);

	//PlayWeaponSound(current_weapon->weapon_sound.fire);

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedFire_animationEnd, d, false);

	FireRangedWeapon();
}

void AHandsActor::FireRangedWeapon()
{
	if (!current_weapon)
	{
		UE_LOG(LogTemp, Error, TEXT("current weapon is null"));
		return;
	}

	current_weapon->Fire();

	updateHUD();
}

void AHandsActor::updateHUD()
{
	
}

void AHandsActor::RangedFire_animationEnd()
{
	RangedEquip_animationEnd();
}

void AHandsActor::F_exit_func()
{
	fire_pressed = false;
}

void AHandsActor::R_enter_func()
{
	isFree = false;

	if (current_weapon->ammo == 0 || current_weapon->current_clip == current_weapon->item_descr->clip_size)
	{
		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedReload_animationEnd, 0.1f, false);

		return;
	}

	if (!current_weapon->weapon_anim.reload || !current_weapon->hands_anim.reload)
	{
		UE_LOG(LogTemp, Error, TEXT("no reload animations"));
		GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedReload_animationEnd, 1, false);
		current_weapon->Reload();

		return;
	}

	float d = URuntimeUtils::PlayMontage(WeaponMesh, current_weapon->weapon_anim.reload);
	URuntimeUtils::PlayMontage(HandsMesh, current_weapon->hands_anim.reload);

	//PlayWeaponSound(current_weapon->weapon_sound.reload);

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &AHandsActor::RangedReload_animationEnd, d, false);

	current_weapon->Reload();
}

void AHandsActor::RangedReload_animationEnd()
{
	RangedEquip_animationEnd();
}

void AHandsActor::R_exit_func()
{
	reload_pressed = false;

	updateHUD();
}