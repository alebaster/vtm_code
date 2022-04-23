#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"



#include "VtmLevelScriptActor.generated.h"

UCLASS()
class VTM_API AVtmLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = LevelBlueprint)
	void OnLevelLoad();
};
