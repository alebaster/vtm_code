#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "RangedWeaponActor.generated.h"

struct FRangedItem;
class UWeaponFx;

USTRUCT(BlueprintType)
struct FRangedWeaponAnimation
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* equip;
    
    UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* idle;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* fire;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* reload;

	UPROPERTY(EditDefaultsOnly)
	UAnimationAsset* hide;
};

USTRUCT(BlueprintType)
struct FRangedWeaponSound
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* equip;
    
    UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* empty;

	UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* shell;

	UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* fire;

	UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* reload;

	UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
	USoundWave* hide;
};

//UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS()
class VTM_API ARangedWeaponActor : public AActor
{
	GENERATED_BODY()

public:
	ARangedWeaponActor();
    
	void Fire();
	void Fire(FVector target);
	void Reload();

	void PlayWeaponSound(USoundWave* snd);

	UPROPERTY(EditDefaultsOnly)
	int ammo=0;
	int current_clip=0;

	UPROPERTY(EditDefaultsOnly)
	bool infinit_ammo=false;

	//UPROPERTY()
	FRangedItem* item_descr=0;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWeaponFx> weapon_fx_bp = 0;
	UPROPERTY()
	UWeaponFx* weapon_fx=0;

    UPROPERTY(EditAnywhere)
    FString id;
    
    UPROPERTY(EditAnywhere)
    USkeletalMeshComponent* mesh=0;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAnimation)
    FRangedWeaponAnimation weapon_anim;
    
    UPROPERTY(EditDefaultsOnly, Category = WeaponAnimation)
    FRangedWeaponAnimation hands_anim;
    
    UPROPERTY(EditDefaultsOnly, Category = WeaponSound)
    FRangedWeaponSound weapon_sound;

protected:
	virtual void BeginPlay() override;
};
