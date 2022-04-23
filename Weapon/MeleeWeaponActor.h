#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "MeleeWeaponActor.generated.h"

struct FMeleeItem;

UCLASS()
class VTM_API AMeleeWeaponAnimation : public AActor
{
	GENERATED_BODY()

public:
	AMeleeWeaponAnimation();
    
	void Attack();

	FMeleeItem* item_descr=0;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UWeaponFx> weapon_fx_bp = 0;

    UPROPERTY(EditAnywhere)
    FString id;
    
    UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* mesh=0;

protected:
	virtual void BeginPlay() override;
};
