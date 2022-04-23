#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Character/WodComponent.h"
#include "UI/DamageWidget.h"

#include "EnemyCharacter.generated.h"

UCLASS()
class VTM_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UPROPERTY(EditAnywhere)
	FString weapon_id;
	UPROPERTY(EditAnywhere)
	FName weapon_socket;

	UPROPERTY(EditAnywhere)
	float rot_speed = 100.0f;
	void Rotate(float r);
	void StopRotation();
	bool IsInRotation();
	bool b_need_rotation=false;

	void Stop();
	bool IsMoving();

	void EqupWeapon();
	void HideWeapon();

	void GotoPoint(FVector& point);

	bool IsAimed(AActor* a);

	bool ReadyToAttack();

	float GetAttackRange();

	void Attack(FVector target);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
							 class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

private:
	// wod
	UPROPERTY()
	UWodComponent* wod_manager=0;

	// UI
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDamageWidget> damage_widget_bp = 0;

	UDamageWidget* damage_widget = 0;

	class ARangedWeaponActor* weapon=0;

	float rot_time=0.0f;
	float desired_rot=0.0f;
	float rot_diff=0.0f;
	FRotator original_rotation;
	void _rotation(float DeltaTime);
	void _widget_move();

	void Death();

	float attack_time = 0.0f;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UFUNCTION()
	void DisableCollision();

public:	
	virtual void Tick(float DeltaTime) override;
};
