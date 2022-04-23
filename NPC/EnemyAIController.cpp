#include "EnemyAIController.h"

#include "EnemyAnimInstance.h"
#include "EnemyCharacter.h"
#include "Character/MainCharacter.h"

#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Animation/AnimNode_StateMachine.h"
#include "NavigationPath.h"

AEnemyAIController::AEnemyAIController()
{
    fsm = CreateDefaultSubobject<UStateMachineComponent>(TEXT("EnemyFSM"));

	perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception Component"));
	sight_config = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	damage_config = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	hear_config = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hear Config"));
	perception->ConfigureSense(*sight_config);
	perception->ConfigureSense(*damage_config);
	perception->ConfigureSense(*hear_config);
	perception->SetDominantSense(sight_config->GetSenseImplementation());
	perception->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::perception_reaction);
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();


}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//sight_config->SightRadius = possessedUnit->sightRange;
	//sight_config->LoseSightRadius = (possessedUnit->sightRange + 20.0f);
	//sight_config->PeripheralVisionAngleDegrees = 360.0f;
	//sight_config->DetectionByAffiliation.bDetectEnemies = true;
	//sight_config->DetectionByAffiliation.bDetectNeutrals = false;
	//sight_config->DetectionByAffiliation.bDetectFriendlies = false;
	//PerceptionComponent->ConfigureSense(*sight_config);
	//UE_LOG(LogTemp, Error, TEXT("OnPossess"));

	if (fsm)
	{
		fsm->RegisterComponent();
		SetupFSM();
	}

	// animation
	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (character)
	{
		anim_inst = Cast<UEnemyAnimInstance>(character->GetMesh()->GetAnimInstance());
		if (!anim_inst)
			UE_LOG(LogTemp, Error, TEXT("anim_inst in enemy character is null"))
	}
}

void AEnemyAIController::OnUnPossess()
{
	Super::OnUnPossess();

	fsm->Stop();
	Destroy();
	//delete fsm;

	UE_LOG(LogTemp, Error, TEXT("OnUnPossess"));
}

void AEnemyAIController::End()
{
	fsm->Stop();
	Destroy();
}

void AEnemyAIController::SetupFSM()
{
    fsm->AddState("chill", &chill_enter_delegate, 0, &chill_exit_delegate);
	fsm->AddState("idle", &idle_enter_delegate, 0, &idle_exit_delegate);
	fsm->AddState("walk", &walk_enter_delegate, &walk_update_delegate, &walk_exit_delegate);
	fsm->AddState("aim", &aim_enter_delegate, &aim_update_delegate, &aim_exit_delegate);
	fsm->AddState("attack", &attack_enter_delegate, 0, &attack_exit_delegate);

	chill_enter_delegate.BindUObject(this, &AEnemyAIController::chill_enter_func);
	chill_exit_delegate.BindUObject(this, &AEnemyAIController::chill_exit_func);

	idle_enter_delegate.BindUObject(this, &AEnemyAIController::idle_enter_func);
	idle_exit_delegate.BindUObject(this, &AEnemyAIController::idle_exit_func);

	walk_enter_delegate.BindUObject(this, &AEnemyAIController::walk_enter_func);
	walk_update_delegate.BindUObject(this, &AEnemyAIController::walk_update_func);
	walk_exit_delegate.BindUObject(this, &AEnemyAIController::walk_exit_func);

	aim_enter_delegate.BindUObject(this, &AEnemyAIController::aim_enter_func);
	aim_update_delegate.BindUObject(this, &AEnemyAIController::aim_update_func);
	aim_exit_delegate.BindUObject(this, &AEnemyAIController::aim_exit_func);

	attack_enter_delegate.BindUObject(this, &AEnemyAIController::attack_enter_func);
	attack_exit_delegate.BindUObject(this, &AEnemyAIController::attack_exit_func);

	fsm->AddTransition("chill", "idle", false, &chill_to_idle_rule_delegate);
	fsm->AddTransition("idle", "chill", false, &idle_to_chill_rule_delegate);
	fsm->AddTransition("idle", "walk", false, &idle_to_walk_rule_delegate);
	fsm->AddTransition("walk", "idle", false, &walk_to_idle_rule_delegate);
	fsm->AddTransition("idle", "aim", false, &idle_to_aim_rule_delegate);
	fsm->AddTransition("aim", "idle", false, &aim_to_idle_rule_delegate);
	fsm->AddTransition("aim", "attack", false, &aim_to_attack_rule_delegate);
	fsm->AddTransition("attack", "aim", false, &attack_to_aim_rule_delegate);

	chill_to_idle_rule_delegate.BindUObject(this, &AEnemyAIController::chill_to_idle_rule);
	idle_to_chill_rule_delegate.BindUObject(this, &AEnemyAIController::idle_to_chill_rule);
	idle_to_walk_rule_delegate.BindUObject(this, &AEnemyAIController::idle_to_walk_rule);
	walk_to_idle_rule_delegate.BindUObject(this, &AEnemyAIController::walk_to_idle_rule);
	idle_to_aim_rule_delegate.BindUObject(this, &AEnemyAIController::idle_to_aim_rule);
	aim_to_idle_rule_delegate.BindUObject(this, &AEnemyAIController::aim_to_idle_rule);
	aim_to_attack_rule_delegate.BindUObject(this, &AEnemyAIController::aim_to_attack_rule);
	attack_to_aim_rule_delegate.BindUObject(this, &AEnemyAIController::attack_to_aim_rule);

	//fsm->AddAbsoluteTransition("hurt",false, &AEnemyAIController::hurt_rule);

	fsm->SetDefaultState("chill");
}

// rules
bool AEnemyAIController::chill_to_idle_rule()
{
	return b_danger;
}
bool AEnemyAIController::idle_to_chill_rule()
{
	return !b_danger;
}

bool AEnemyAIController::idle_to_walk_rule()
{
	if (!player_actor)
	{
		//return false;
		return !b_know_where;
	}

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	float dist = FVector::Dist(character->GetActorLocation(), player_actor->GetActorLocation());

	return ((dist - character->GetAttackRange()) > 0) || !b_know_where;
}

bool AEnemyAIController::walk_to_idle_rule()
{
	if (!player_actor)
		return true;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	float dist = FVector::Dist(character->GetActorLocation(), player_actor->GetActorLocation());

	//UE_LOG(LogTemp, Error, TEXT("dist : %d"), (dist - character->GetAttackRange()) <= 0);
	
	//UE_LOG(LogTemp, Error, TEXT("moving : %d"), character->IsMoving());

	UNavigationPath* path = UAIBlueprintHelperLibrary::GetCurrentPath(this);
	//if (!path)
	//	return ((dist - character->GetAttackRange()) <= 0) && !character->IsMoving();
	//else
	//return ((dist - character->GetAttackRange()) <= 0) && !path;

	FVector pos = character->GetActorLocation();
	return ((dist - character->GetAttackRange()) <= 0) && is_enemy_in_sight(pos);
}

bool AEnemyAIController::idle_to_aim_rule()
{
	if (!player_actor)
		return false;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	float dist = FVector::Dist(character->GetActorLocation(), player_actor->GetActorLocation());

	FVector pos = character->GetActorLocation();
	//return ((dist - character->GetAttackRange()) <= 0) && character->IsAimed(player_actor);
	return ((dist - character->GetAttackRange()) <= 0) && is_enemy_in_sight(pos);
}

bool AEnemyAIController::aim_to_idle_rule()
{
	if (!player_actor)
		return true;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	float dist = FVector::Dist(character->GetActorLocation(), player_actor->GetActorLocation());

	//UE_LOG(LogTemp, Error, TEXT("dist : %f"), dist - character->GetAttackRange());
	//UE_LOG(LogTemp, Error, TEXT("know : %d"), b_know_where);

	return ((dist - character->GetAttackRange()) > 0) || !b_know_where;
}

bool AEnemyAIController::aim_to_attack_rule()
{
	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());

	FVector pos = character->GetActorLocation();
	//return character->IsAimed(player_actor) && character->ReadyToAttack();
	return is_enemy_in_sight(pos) && character->ReadyToAttack();
}

bool AEnemyAIController::attack_to_aim_rule()
{
	if (!anim_inst)
		return true;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (!character)
		return true;

	//UAnimMontage* mon = anim_inst->GetCurrentActiveMontage();
	//if (mon == 0)
	//	return true;

	FAnimNode_StateMachine* BaseStateMachine = anim_inst->GetStateMachineInstanceFromName(FName("Root"));
	auto indexBaseStateMachine = anim_inst->GetStateMachineIndex(FName("Root"));
	float TimeRemaining = anim_inst->GetRelevantAnimTimeRemaining(indexBaseStateMachine, BaseStateMachine->GetCurrentState());

	return TimeRemaining == 0.0f;
}

bool AEnemyAIController::hurt_rule()
{
	return false;
}
//

// actions
void AEnemyAIController::chill_enter_func()
{

}
void AEnemyAIController::chill_exit_func()
{

}
void AEnemyAIController::idle_enter_func()
{
	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (character)
		character->EqupWeapon();
}
void AEnemyAIController::idle_exit_func()
{
	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (character)
		character->HideWeapon();
}

void AEnemyAIController::walk_enter_func()
{
	//FVector player_location = last_known_point;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());

	//const FVector SearchOrigin = player_actor->GetActorLocation() + min * (bot->GetActorLocation() - enemy->GetActorLocation()).GetSafeNormal();
	//FVector point = UNavigationSystemV1::GetRandomReachablePointInRadius(this, player_location, character->GetAttackRange());
	//FVector point = UNavigationSystemV1::GetRandomReachablePointInRadius(this, player_actor->GetActorLocation(), character->GetAttackRange());

	//character->GotoPoint(point);
	//UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	////UNavigationSystemV1* NavSys = GetWorld()->GetNavigationSystem();
	//if (NavSys)
	//	NavSys->SimpleMoveToLocation(this, point);

	


  //DrawDebugSphere(const UWorld * InWorld, FVector const& Center, float Radius, int32 Segments, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) {}

	FVector point;

	float range = character->GetAttackRange();
	do
	{
		FVector V = player_actor->GetActorLocation() - character->GetActorLocation();
		V.Normalize();
		FVector X = player_actor->GetActorLocation() - range * V;
		//point = UNavigationSystemV1::GetRandomReachablePointInRadius(this, X, 10.0f);
		point = UNavigationSystemV1::GetRandomReachablePointInRadius(this, player_actor->GetActorLocation(), range);

		range -= 25.0f;

		if (range <= 0.0f)
			break;
	} while (!is_enemy_in_sight(point));

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, point);

	//DrawDebugSphere(GetWorld(), point, 10, 26, FColor(181, 0, 0), true, -1, 0, 2);

	UNavigationPath* path = UAIBlueprintHelperLibrary::GetCurrentPath(this);
	if (path)
	{
		UE_LOG(LogTemp, Warning, TEXT("goto %s"), *point.ToString());
	}
	//else
	//	walk_enter_func();

	//if (path)
	//	UE_LOG(LogTemp, Warning, TEXT("path len %d"), path->PathPoints.Num())
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("path Is NULL"));
}

void AEnemyAIController::walk_update_func()
{
	//walk_enter_func();
}

void AEnemyAIController::walk_exit_func()
{
	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	character->Stop();
}

void AEnemyAIController::aim_enter_func()
{
	if (anim_inst)
		anim_inst->isAiming = true;
	else 
		UE_LOG(LogTemp, Error, TEXT("PIZDA"));

	if (player_actor)
	{
		//FVector v = perception->GetActorLocation(*player_actor);
		//UE_LOG(LogTemp, Warning, TEXT("actor loc %s"), *v.ToString());
	}

	aim_update_func();
}

void AEnemyAIController::aim_update_func()
{
	if (!player_actor)
		return;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (!character)
		return;
	FVector pos = character->GetActorLocation();
	//FVector victim = perception->GetActorLocation(*player_actor);
	FVector victim = player_actor->GetActorLocation();
	FVector direction = victim - pos;
	direction.Normalize();

	FRotator newrot = direction.Rotation();
	float angle = newrot.Yaw + 15.0f;

	character->Rotate(angle);
}

void AEnemyAIController::aim_exit_func()
{
	if (anim_inst)
		anim_inst->isAiming = false;

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	character->StopRotation();
}

void AEnemyAIController::attack_enter_func()
{
	if (anim_inst)
	{
		anim_inst->isAiming = true;
		anim_inst->isAttacking = true;
	}

	AEnemyCharacter* character = Cast<AEnemyCharacter>(GetCharacter());
	if (character)
	{
		character->Attack(player_actor->GetActorLocation());
		//auto m = player_actor->GetMesh();
		//if (m)
		//{
		//	FVector pos = m->GetSocketTransform("Bip01-Spine");
		//	character->Attack(pos);
		//}
	}
}

void AEnemyAIController::attack_exit_func()
{
	if (anim_inst)
		anim_inst->isAttacking = false;
}
//

void AEnemyAIController::perception_reaction(const TArray<AActor*>& actors)
{
	b_know_where = false;

	TArray<AActor*> _actors;
	//perception->GetCurrentlyPerceivedActors(0,_actors);
	perception->GetKnownPerceivedActors(0, _actors);
	UE_LOG(LogTemp, Warning, TEXT("actors %d"), _actors.Num());
	//player_actor = 0;
	//UE_LOG(LogTemp, Warning, TEXT("player_actor = 0"));
	for (auto& a : _actors)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"),*a->GetActorLabel());
		AMainCharacter* c = Cast<AMainCharacter>(a);
		if (c)
		{
			UE_LOG(LogTemp, Warning, TEXT("I CAN FEEL YOU: %s"), *GetName());
			b_danger = true;
			//b_can_shoot = true;
			player_actor = c;
			b_know_where = true;
			last_known_point = perception->GetActorLocation(*player_actor);
		}
		//else
		//{
		//	UE_LOG(LogTemp, Error, TEXT("I LOST YOU"));
		//	b_know_where = false;
		//}
	}

	//if (player_actor)
	//{
	//	auto info = perception->GetActorInfo(*player_actor);
	//	//if (!info->HasAnyCurrentStimulus())
	//	//	player_actor = 0;
	//	//UE_LOG(LogTemp, Warning, TEXT("kegg: %d"), info->HasAnyCurrentStimulus());
	//}

	//if (!player_actor)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("lost player actor"));
	//}

	if (player_actor)
	{
		const FActorPerceptionInfo* info = perception->GetActorInfo(*player_actor);
		if (info)
		{
			for (const FAIStimulus& Stimulus : info->LastSensedStimuli)
			{
				if (Stimulus.IsExpired())
				{
					UE_LOG(LogTemp, Warning, TEXT("lost player actor"));
					b_know_where = false;
				}
			}
		}

		//if (info->HasAnyKnownStimulus())
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("!!!!!! lost player actor"));
		//	b_know_where = false;
		//}
	}
}

bool AEnemyAIController::is_enemy_in_sight(FVector& point)
{
	if (!player_actor)
		return false;

	FVector target = player_actor->GetActorLocation();
	target.Z += 25.0f;
	FVector origin = point;
	point.Z += 25.0f;
	FVector AimDirection = target - origin;
	AimDirection.Normalize();
	FVector StartAim = origin;
	FVector ShootDirection = AimDirection;
	FVector EndTrace = StartAim + ShootDirection * 500;

	FHitResult Impact(ForceInit);

	FName WeaponFireTag = FName(TEXT("WeaponTrace"));
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	//TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(this);
	TraceParams.bIgnoreBlocks = false;

	//DrawDebugLine(this->GetWorld(), StartAim, EndTrace, FColor::Red, true, 10000, 10.0f);
	GetWorld()->LineTraceSingleByChannel(Impact, StartAim, EndTrace, ECollisionChannel::ECC_Visibility, TraceParams);

	return Impact.GetActor() == player_actor;
}