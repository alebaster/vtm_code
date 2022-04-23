#include "StateMachineComponent.h"

#include "StateObject.h"

//#include "Lua/LuaComponent.h"

UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStateMachineComponent::Stop()
{
	SetComponentTickEnabled(false);

	UE_LOG(LogTemp, Error, TEXT("STOP"));

	_b_is_active = false;
	//current_state = 0;
	//for (auto p : all_states)
	//{
	//
	//}
}

void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UStateMachineComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!_b_is_active)
		return;

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateMachine();
}

void UStateMachineComponent::SetDefaultState(FString id)
{
	UStateObject** sop = all_states.Find(id);
	if (!sop)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot find state with id %s"), *id);
		return;
	}

	current_state = *sop;
}

void UStateMachineComponent::AddState(FString id, FExecDelegate* enter, FExecDelegate* update, FExecDelegate* exit, FString parent)
{
	UStateObject* so;
	so = NewObject<UStateObject>();
	so->parent = parent;
	//so->enter_func = MakeShareable(enter);
	//so->update_func = MakeShareable(update);
	//so->exit_func = MakeShareable(exit);
	so->enter_func = enter;
	so->update_func = update;
	so->exit_func = exit;

	all_states.Add(id, so);
}

void UStateMachineComponent::AddTransition(FString from, FString to, bool auto_exit, FTransitionRuleDelegate* rule)
{
	UStateObject** sop = all_states.Find(from);
	if (!sop)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot find state with id %s"), *from);
		return;
	}

	UStateObject* so = *sop;
	so->AddTransition(to, auto_exit, rule);

	// ???
	//if (bidirectional)
}

void UStateMachineComponent::AddAbsoluteTransition(FString to, bool auto_exit, FTransitionRuleDelegate* rule)
{
	for (auto& so : all_states)
	{
		so.Value->AddTransition(to,auto_exit,rule);
	}
}

void UStateMachineComponent::UpdateMachine()
{
	if (!current_state)
		return;

	FString next_state;

	if (current_state->CheckTransitions(next_state))
	{
		current_state->Exit();

		UStateObject** sop = all_states.Find(next_state);
		if (!sop)
		{
			UE_LOG(LogTemp, Error, TEXT("cannot find state with id %s"), *next_state);
			return;
		}

		current_state = *sop;
		current_state->Enter();
		UE_LOG(LogTemp, Warning, TEXT("new state %s"), *next_state);
	}
	else
		current_state->Update();
}