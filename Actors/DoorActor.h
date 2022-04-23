#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"

#include "Actors/UsableActor.h"

#include "DoorActor.generated.h"

class ULuaComponent;

UCLASS()
class VTM_API ADoorActor : public AActor, public IUsableActor
{
	GENERATED_BODY()
	
public:	
	ADoorActor();

	void PlayLockpicking();
	void PlayLockpickingSuccess();
	void PlayLockpickingFail();

	UPROPERTY(EditAnywhere)
	UINT8 Difficulty=0;

	//UPROPERTY(EditAnywhere)
	//UBoxComponent* trigger_box;

	UFUNCTION()
	virtual UTexture2D* GetActionImage() override;

	//UFUNCTION()
	//virtual bool IsAvaible() override;

	UFUNCTION()
	virtual int Use(AActor* who) override;

	UFUNCTION()
	void Unlock() { locked = false; }

	UPROPERTY(EditAnywhere)
	FString key_id;

	UPROPERTY(EditAnywhere)
	bool locked = false;

	UPROPERTY(EditAnywhere)
	float speed = 150;

	UPROPERTY(EditAnywhere)
	UTexture2D* DenyImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* ActionImage;

	UPROPERTY(EditAnywhere)
	USoundWave* OpenSound;

	UPROPERTY(EditAnywhere)
	USoundWave* CloseSound;

	UPROPERTY(EditAnywhere)
	USoundWave* LockpickingSound;

	UPROPERTY(EditAnywhere)
	USoundWave* LockpickingSuccessSound;

	UPROPERTY(EditAnywhere)
	USoundWave* LockpickingFailSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* trigger_box;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:	
	UPROPERTY()
	USceneComponent* left_root_component = 0;
	UPROPERTY()
	USceneComponent* right_root_component = 0;

	FVector forward;

	bool b_need_rotation=false;
	bool state=false; // 0 - closed, 1 - opened
	FRotator original_rotation_left;
	FRotator original_rotation_right;
	double start_time=0.0f;
	float angle_left=0;
	float angle_right=0;
	void Rotate(float DeltaTime);

	UFUNCTION()
	void AutoClose();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	AActor* who_actor = 0;

	UPROPERTY()
	ULuaComponent* lua=0;
};
