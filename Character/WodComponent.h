#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"



#include "WodComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UWodComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWodComponent();

	int GetHP();
	int ChangeHP(int amount);
	void GiveExp(int amount);

private:
	int hp = 100;

protected:
	virtual void BeginPlay() override;
};
