// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "vtmGameMode.h"
#include "UObject/ConstructorHelpers.h"

AvtmGameMode::AvtmGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Blueprint/VtmPlayerController_bp"));
	if (PlayerControllerBPClass.Class != NULL)
		PlayerControllerClass = PlayerControllerBPClass.Class;

	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/MainCharacter_bp2"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
