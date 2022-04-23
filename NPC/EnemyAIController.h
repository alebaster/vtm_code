#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"

//#include "BehaviorTree/BehaviorTree.h"
//#include "BehaviorTree/BlackboardComponent.h"

#include "StateMachine/StateMachineComponent.h"

#include "EnemyAIController.generated.h"

class UEnemyAnimInstance;

UCLASS()
class VTM_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	void End();

	//UPROPERTY(EditAnywhere, Category = "AI")
	//UBehaviorTree* BehaviorTree=0;
	//UBlackboardComponent* Blackboard=0;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	void SetDanger(bool d) { b_danger = d; }

private:
	// fsm stuff
	UPROPERTY()
	UStateMachineComponent* fsm = 0;
	void SetupFSM();

	FTransitionRuleDelegate chill_to_idle_rule_delegate;
	FTransitionRuleDelegate idle_to_chill_rule_delegate;
	FTransitionRuleDelegate idle_to_walk_rule_delegate;
	FTransitionRuleDelegate walk_to_idle_rule_delegate;
	FTransitionRuleDelegate idle_to_aim_rule_delegate;
	FTransitionRuleDelegate aim_to_idle_rule_delegate;
	FTransitionRuleDelegate aim_to_attack_rule_delegate;
	FTransitionRuleDelegate attack_to_aim_rule_delegate;

	bool chill_to_idle_rule();
	bool idle_to_chill_rule();
	bool idle_to_walk_rule();
	bool walk_to_idle_rule();
	bool idle_to_aim_rule();
	bool aim_to_idle_rule();
	bool aim_to_attack_rule();
	bool attack_to_aim_rule();
	bool hurt_rule();

	FExecDelegate chill_enter_delegate;
	FExecDelegate chill_exit_delegate;

	FExecDelegate idle_enter_delegate;
	FExecDelegate idle_exit_delegate;

	FExecDelegate walk_enter_delegate;
	FExecDelegate walk_update_delegate;
	FExecDelegate walk_exit_delegate;

	FExecDelegate aim_enter_delegate;
	FExecDelegate aim_update_delegate;
	FExecDelegate aim_exit_delegate;

	FExecDelegate attack_enter_delegate;
	FExecDelegate attack_exit_delegate;

	void chill_enter_func();
	void chill_exit_func();

	void idle_enter_func();
	void idle_exit_func();

	void walk_enter_func();
	void walk_update_func();
	void walk_exit_func();

	void aim_enter_func();
	void aim_update_func();
	void aim_exit_func();

	void attack_enter_func();
	void attack_exit_func();

	bool b_danger=false;
	bool b_can_attack=false;
	bool b_know_where=false;

	FVector last_known_point;

	// animation
	UEnemyAnimInstance* anim_inst;

	// perception
	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* perception=0;

	UPROPERTY(EditAnywhere)
	UAISenseConfig_Sight* sight_config=0;
	UPROPERTY(EditAnywhere)
	UAISenseConfig_Damage* damage_config=0;
	UPROPERTY(EditAnywhere)
	UAISenseConfig_Hearing* hear_config=0;

	AActor* player_actor=0;

	UFUNCTION()
	void perception_reaction(const TArray<AActor*>& actors);

	bool is_enemy_in_sight(FVector& point);
};
