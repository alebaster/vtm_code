#include "EnemyCharacter.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Weapon/RangedWeaponActor.h"
#include "VtmPlayerController.h"
#include "Inventory/InventoryComponent.h"
#include "EnemyAnimInstance.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	wod_manager = CreateDefaultSubobject<UWodComponent>(TEXT("WodManager"));
	if (!wod_manager)
		UE_LOG(LogTemp, Error, TEXT("wod_manager is null"))
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	_rotation(DeltaTime);

	_widget_move();
}

void AEnemyCharacter::Rotate(float r)
{
	//FRotator target_rotation = FRotator(GetActorRotation().Pitch, r, GetActorRotation().Roll);
	//SetActorRotation(target_rotation);

	rot_time = 0;
	desired_rot = r;
	original_rotation = GetActorRotation();
	rot_diff = FMath::Abs(desired_rot - GetActorRotation().Yaw);
	b_need_rotation = true;
}

void AEnemyCharacter::StopRotation()
{
	rot_time = 0;
	desired_rot = GetActorRotation().Yaw;
	b_need_rotation = false;
}

bool AEnemyCharacter::IsInRotation()
{
	//return desired_rot != GetActorRotation().Yaw;
	return b_need_rotation;
}

void AEnemyCharacter::Stop()
{
	UCharacterMovementComponent* move_comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (move_comp)
		return move_comp->StopMovementImmediately();
}

bool AEnemyCharacter::IsMoving()
{
	UCharacterMovementComponent* move_comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (move_comp)
		return move_comp->IsMovementInProgress();

	return false;
}

bool AEnemyCharacter::IsAimed(AActor* a)
{
	if (!weapon)
		return false;

	if (!a)
		return false;

	FVector target = a->GetActorLocation();
	target.Z += 25.0f;
	FTransform StartTransform = weapon->mesh->GetSocketTransform("FireSocket");
	FVector AimDirection = target - StartTransform.GetLocation();
	AimDirection.Normalize();
	FVector StartAim = StartTransform.GetLocation();
	FVector ShootDirection = AimDirection;
	FVector EndTrace = StartAim + ShootDirection * 500;

	FHitResult Impact(ForceInit);

	FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(Impact, StartAim, EndTrace, ECollisionChannel::ECC_Pawn, TraceParams);

	//DrawDebugLine(this->GetWorld(), StartAim, EndTrace, FColor::Red, true, 10000, 10.0f);
	//UE_LOG(LogTemp, Error, TEXT("aimed to : %s"), *Impact.GetActor()->GetName());

	return Impact.GetActor() == a;
}

bool AEnemyCharacter::ReadyToAttack()
{
	return !IsInRotation() && (GetWorld()->GetRealTimeSeconds()-attack_time >= 2.0f);
}

void AEnemyCharacter::EqupWeapon()
{
	FString path = "/Game/Blueprint/weapons/";
	UClass* bp = URuntimeUtils::GetBlueprint<ARangedWeaponActor>(path, weapon_id);
	if (!bp)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot get ranged weapon blueprint with id: %s"), *weapon_id);
		return;
	}

	FTransform t;
	FActorSpawnParameters sp;
	t.SetRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
	t.SetLocation(FVector::ZeroVector);
	//sp.Name = *weapon_id;
	sp.Owner = this;
	ARangedWeaponActor* obj = Cast<ARangedWeaponActor>(GetWorld()->SpawnActor(bp->GetDefaultObject()->GetClass(), &t, sp));
	if (obj)
	{
		// huevo
		AVtmPlayerController* pc = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (!pc)
			return;
		UInventoryComponent* inv = Cast<UInventoryComponent>(pc->GetComponentByClass(UInventoryComponent::StaticClass()));
		if (!inv)
			return;
		FRangedItem* w = static_cast<FRangedItem*>(inv->GetItemFromDB(weapon_id));
		if (!w)
			return;
		//obj->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
		obj->item_descr = w;
		//obj->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, weapon_socket);
		obj->AttachToComponent(Cast<USceneComponent>(GetMesh()), FAttachmentTransformRules::SnapToTargetNotIncludingScale, weapon_socket);
		//FVector v = GetMesh()->GetSocketLocation(weapon_socket);
		weapon = obj;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("failed to spawn weapon with id: %s"), *weapon_id);
		return;
	}
}

void AEnemyCharacter::GotoPoint(FVector& point)
{
	//UCharacterMovementComponent* move_comp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	//if (move_comp)
	//{
	//	move_comp->set
	//}
	//
	//UNavigationSystem* NavSys = GetWorld()->GetNavigationSystem();
	//if (NavSys)
	//	NavSys->SimpleMoveToLocation(, point);
}

void AEnemyCharacter::HideWeapon()
{

}

float AEnemyCharacter::GetAttackRange()
{
	// temp
	return 600.0f;
}

void AEnemyCharacter::Attack(FVector target)
{
	if (weapon)
		weapon->Fire(target);

	attack_time = GetWorld()->GetRealTimeSeconds();
	UE_LOG(LogTemp, Error, TEXT("enemy attack"));
}

float AEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount,DamageEvent,EventInstigator, DamageCauser);

	int hp = wod_manager->ChangeHP(-DamageAmount);
	if (hp == 0)
		Death();
	else
	{
		UEnemyAnimInstance* anim_inst = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());
		if (anim_inst)
			anim_inst->isHitHead = true;
	}

	// ui
	if (!damage_widget)
	{
		if (damage_widget_bp)
			damage_widget = CreateWidget<UDamageWidget>(GetWorld(), damage_widget_bp);
		else
			UE_LOG(LogTemp, Error, TEXT("damage_widget_bp is null"));
	}

	if (damage_widget)
	{
		damage_widget->SetAmount(DamageAmount);
		if (!damage_widget->IsInViewport())
			damage_widget->AddToViewport();
		damage_widget->Play();
	}
	
	return 0.0f;
}

void AEnemyCharacter::_rotation(float DeltaTime)
{
	//if (desired_rot != GetActorRotation().Yaw)
	if (b_need_rotation)
	{
		FRotator target_rotation = FRotator(GetActorRotation().Pitch, desired_rot, GetActorRotation().Roll);

		rot_time += DeltaTime;
		float distance = rot_speed * (rot_time);
		float alpha = distance / rot_diff;
		//UE_LOG(LogTemp, Warning, TEXT("1: %f"), GetActorRotation().Yaw);
		//UE_LOG(LogTemp, Warning, TEXT("2: %f"), desired_rot);
		SetActorRotation(FMath::Lerp(original_rotation, target_rotation, alpha));

		if (FMath::Abs(distance - rot_diff) < 3) // ??
		{
			SetActorRotation(target_rotation);
			desired_rot = GetActorRotation().Yaw;
			b_need_rotation = false;
		}
	}
}

void AEnemyCharacter::_widget_move()
{
	if (!damage_widget)
		return;

	APlayerController* pc =  UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!pc)
		return;

	FVector2D v;
	pc->ProjectWorldLocationToScreen(GetActorLocation(),v);

	damage_widget->SetPositionInViewport(v);
}

void AEnemyCharacter::DisableCollision()
{
	//GetMesh()->SetAllBodiesSimulatePhysics(false);
	//GetMesh()->SetSimulatePhysics(false);
	//GetMesh()->PutAllRigidBodiesToSleep();
	//GetMesh()->bBlendPhysics = false;

	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

// huevo
void AEnemyCharacter::Death()
{
	// movement
	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	//GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	//SetActorEnableCollision(true);

	// ragdoll
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	if (Controller)
		Controller->UnPossess();

	AEnemyAIController* ai = Cast<AEnemyAIController>(Controller);
	if (ai)
		ai->End();

	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter::DisableCollision, 2.0f, false);
}