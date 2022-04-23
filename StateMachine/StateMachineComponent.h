#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "StateMachineComponent.generated.h"

class UStateObject;

DECLARE_DELEGATE_RetVal(bool, FTransitionRuleDelegate);

DECLARE_DELEGATE(FExecDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateMachineComponent();

	void Stop();

	void SetDefaultState(FString id);
	void AddState(FString id, FExecDelegate* enter=0, FExecDelegate* update=0, FExecDelegate* exit=0, FString parent="");
	void AddTransition(FString from, FString to, bool auto_exit=false, FTransitionRuleDelegate* rule=0);
	void AddAbsoluteTransition(FString to, bool auto_exit, FTransitionRuleDelegate* rule = 0);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool _b_is_active=true;

	UPROPERTY()
	UStateObject* current_state=0;
	UPROPERTY()
	TMap<FString, UStateObject*> all_states;

	void UpdateMachine();
};
