#include "StateObject.h"

UStateObject::UStateObject()
{

}

UStateObject::~UStateObject()
{
	//UE_LOG(LogTemp, Error, TEXT("DETSr"));

	//enter_func.Reset();
	//update_func.Reset();
	//exit_func.Reset();
}

bool UStateObject::CheckTransitions(FString& s)
{
	for (auto p : trainsitions)
	{
		if (p.Key)
		{
			if (p.Key->IsBound())
			{
				if (p.Key->Execute())
				{
					s = p.Value;
					return true;
				}
			}
		}
	}

	return false;
}

void UStateObject::Enter()
{
	//if (enter_func.IsValid())
	if (enter_func)
		enter_func->ExecuteIfBound();
}

void UStateObject::Update()
{
	//if (update_func.IsValid())
	if (update_func)
		update_func->ExecuteIfBound();
}

void UStateObject::Exit()
{
	//if (exit_func.IsValid())
	if (exit_func)
		exit_func->ExecuteIfBound();
}

void UStateObject::AddTransition(FString& to, bool auto_exit, FTransitionRuleDelegate* rule)
{
	trainsitions.Add(MakeTuple(rule,to));
}