#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "CharacterAnimInstance.generated.h"

UCLASS()
class VTM_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void SetRunning();
	void SetWalking();
	void SetCrouching();

	void UpdateMovementType();

	void WalkPressed();
	void WalkReleased();

	void CrouchPressed();

	void JumpPressed();

	void WPress();
	void SPress();
	void DPress();
	void APress();

	void WRelease();
	void SRelease();
	void DRelease();
	void ARelease();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isRunning=true; // default

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isWalking=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCrouching=false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isJumping=false;

	// directions (buttons)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isW = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isS = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isA = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isD = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isWA = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isWD = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSA = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isSD = false;

	UFUNCTION(BlueprintCallable)
	void JumpEndBeginNotyfyFromBP();

	UFUNCTION(BlueprintCallable)
	void JumpEndEndNotyfyFromBP();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> CarpetSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> ConcreteSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> GroundSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> MetalSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> SandSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> WaterSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> WoodSounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USoundWave*> TileSounds;

	UFUNCTION(BlueprintCallable)
	void StepNotyfyFromBP();

private:
	void ClearWASD();
};
