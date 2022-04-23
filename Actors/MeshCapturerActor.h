#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"



#include "MeshCapturerActor.generated.h"

UCLASS()
class VTM_API AMeshCapturerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshCapturerActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
