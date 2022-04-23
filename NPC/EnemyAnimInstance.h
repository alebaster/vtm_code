#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"



#include "EnemyAnimInstance.generated.h"

UCLASS()
class VTM_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isInStance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isReloading = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHitHead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHitLow = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHitLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHitRight = false;
};
