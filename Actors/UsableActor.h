#pragma once

#include "Runtime/CoreUObject/Public/UObject/Interface.h"

#include "UsableActor.generated.h"

UINTERFACE()
class UUsableActor : public UInterface
{
	GENERATED_BODY()
};

class IUsableActor
{
	GENERATED_BODY()

public:	
	UFUNCTION()
	virtual FString GetActionString();

	UFUNCTION()
	virtual UTexture2D* GetActionImage();

	// 0 if instant (door, pickup, buttons)
	// 1 if reuse or esc (notes)
	// 2 if command within (dialog)
	// 3 if auto (lockpicking)
	// 4 if command within or esc (hacking)
	// 5 if mouse click or esc (barter) [need mouse]
	UFUNCTION()
	virtual int Use(AActor* who); 

	UFUNCTION()
	virtual void EndUse(AActor* who);

	UFUNCTION()
	virtual bool IsAvaible();
};

