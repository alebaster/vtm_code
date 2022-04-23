#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "StateMachineComponent.h"

#include "StateObject.generated.h"

typedef TSharedPtr<FExecDelegate> smartExecDeleg;

UCLASS()
class VTM_API UStateObject : public UObject
{
	GENERATED_BODY()

public:	
	UStateObject();
	~UStateObject();

	bool CheckTransitions(FString& s);

	void Enter();
	void Update();
	void Exit();

	void AddTransition(FString& to, bool auto_exit=false, FTransitionRuleDelegate* rule=0);
	//FTransitionRuleDelegate* AddTransition(FString& to, bool auto_exit = false);

	FString id="";
	FString parent="";

	//smartExecDeleg enter_func=0;
	//smartExecDeleg update_func=0;
	//smartExecDeleg exit_func=0;

	FExecDelegate* enter_func=0;
	FExecDelegate* update_func=0;
	FExecDelegate* exit_func=0;

protected:

private:
	TArray<TPair<FTransitionRuleDelegate*,FString>> trainsitions;
	//TArray<FTransitionRuleDelegate*, FString> trainsitions;
	//FTransitionRuleDelegate transition_rule;
	
	//ExecuteIfBound
};
