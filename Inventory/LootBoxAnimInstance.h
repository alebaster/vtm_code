#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"



#include "LootBoxAnimInstance.generated.h"

UCLASS()
class VTM_API ULootBoxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isUse = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool isEndUse = false;
};
