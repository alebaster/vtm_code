#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "StateMachine/StateMachineComponent.h"
#include "Inventory/InventoryItem.h"
#include "Weapon/RangedWeaponActor.h"

#include "HandsActor.generated.h"

USTRUCT(BlueprintType)
struct FLockPickAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* equip;
    
	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* start;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* pick;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* end;
};

//UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS()
class VTM_API AHandsActor : public AActor
{
	GENERATED_BODY()

public:	
	AHandsActor();

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* HandsMesh=0;

	UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* LockPickMesh=0;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* WeaponMesh=0;
    
	// make private?
    bool isFree=true;
	bool equip_pressed=false;
	bool hide_pressed=false;
	bool reload_pressed=false;
	bool fire_pressed=false;
	bool equip_new=false;

	void EquipRangedWeapon(ARangedWeaponActor* weapon);

	void EquipLockpick();
	void EndLockpick();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* capsule=0;

	UPROPERTY(EditDefaultsOnly, Category = LockPickAnimation)
	FLockPickAnimation LockPickAnimation;
    
    UPROPERTY(EditDefaultsOnly, Category = LockPickAnimationHand)
    FLockPickAnimation LockPickAnimationHands;
    
    void FreeHands();
    
	// lockpicks
	UFUNCTION()
    void LockPickEquipEnd();
	UFUNCTION()
    void PickLockPick();
	UFUNCTION()
	void EndLockPickAnimation();

	// ranged weapon
	UFUNCTION()
	void RangedEquip_animationEnd();
	UFUNCTION()
	void RangedHide_animationEnd();
	UFUNCTION()
	void RangedFire_animationEnd();
	UFUNCTION()
	void RangedReload_animationEnd();

    //float PlayMontage(USkeletalMeshComponent* me, UAnimationAsset* a, bool loop=false);
    //void StopMontage(USkeletalMeshComponent* me, UAnimationAsset* a);

	void PlayWeaponSound(USoundWave* snd);
    
    FTimerHandle montage_TimerHandle;

	void FireRangedWeapon();
	void updateHUD();

private:
	ARangedWeaponActor* current_weapon=0;

	// fsm stuff
	UPROPERTY()
	UStateMachineComponent* fsm=0;
	void SetupFSM();

	// make smart and local?
	FTransitionRuleDelegate HtoE_rule_delegate;
	FTransitionRuleDelegate	EtoI_rule_delegate;
	FTransitionRuleDelegate ItoH_rule_delegate;
	FTransitionRuleDelegate ItoF_rule_delegate;
	FTransitionRuleDelegate FtoI_rule_delegate;
	FTransitionRuleDelegate ItoR_rule_delegate;
	FTransitionRuleDelegate RtoI_rule_delegate;
	FTransitionRuleDelegate ItoE_rule_delegate;

	bool HtoE_rule();
	bool ItoH_rule();
	bool ItoF_rule();
	bool FtoI_rule();
	bool ItoR_rule();
	bool RtoI_rule();
	bool EtoI_rule();
	bool ItoE_rule();

	FExecDelegate H_enter_delegate;
	//FExecDelegate H_update_delegate;
	FExecDelegate H_exit_delegate;

	FExecDelegate E_enter_delegate;
	//FExecDelegate E_update_delegate;
	FExecDelegate E_exit_delegate;

	FExecDelegate I_enter_delegate;
	//FExecDelegate I_update_delegate;
	FExecDelegate I_exit_delegate;

	FExecDelegate F_enter_delegate;
	//FExecDelegate F_update_delegate;
	FExecDelegate F_exit_delegate;

	FExecDelegate R_enter_delegate;
	//FExecDelegate R_update_delegate;
	FExecDelegate R_exit_delegate;

	void H_enter_func();
	void H_exit_func();

	void E_enter_func();
	void E_exit_func();

	void I_enter_func();
	void I_exit_func();

	void F_enter_func();
	void F_exit_func();

	void R_enter_func();
	void R_exit_func();
};
