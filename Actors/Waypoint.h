#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"



#include "Waypoint.generated.h"

UCLASS()
class VTM_API AWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AWaypoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
