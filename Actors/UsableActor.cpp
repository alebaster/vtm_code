#include "Actors/UsableActor.h"

FString IUsableActor::GetActionString()
{
	return "";
}

UTexture2D* IUsableActor::GetActionImage()
{
	return 0;
}

int IUsableActor::Use(AActor* who)
{
	(void)who;
    
    return 0;
}

void IUsableActor::EndUse(AActor* who)
{
	(void)who;
}

bool IUsableActor::IsAvaible()
{
	return true;
}