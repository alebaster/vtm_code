#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Actors/UsableActor.h"

#include "PickableActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UPickableActorComponent : public UActorComponent, public IUsableActor
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
    FString item_id="";

	UPROPERTY(EditAnywhere)
    int amount=1;

	UPROPERTY(EditAnywhere)
	FString lua_script;

	UPickableActorComponent();
    
    UFUNCTION()
	virtual UTexture2D* GetActionImage() override;

	UFUNCTION()
	virtual int Use(AActor* who) override;

protected:
	virtual void BeginPlay() override;
    
private:
	UPROPERTY(EditAnywhere)
	UTexture2D* ActionImage=0;
	
};
