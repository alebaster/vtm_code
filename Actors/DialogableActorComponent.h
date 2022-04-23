#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"

#include "Actors/UsableActor.h"

#include "DialogableActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UDialogableActorComponent : public UActorComponent, public IUsableActor
{
	GENERATED_BODY()

public:	
	UDialogableActorComponent();

	UFUNCTION()
	virtual UTexture2D* GetActionImage() override;

	UFUNCTION()
	virtual int Use(AActor* who) override;

	UFUNCTION()
	virtual void EndUse(AActor* who) override;

	UPROPERTY(EditAnywhere)
	FString id;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* intro_anim=0;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* dialog_anim=0;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* end_anim=0;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UTexture2D* ActionImage;

	FTimerHandle montage_TimerHandle;

	AActor* _who=0;

	UFUNCTION()
	void IntroAnimationEnded();
};
