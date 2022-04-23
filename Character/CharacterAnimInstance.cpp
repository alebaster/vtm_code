#include "CharacterAnimInstance.h"

#include "Kismet/GameplayStatics.h"

#include "MainCharacter.h"

void UCharacterAnimInstance::WalkPressed()
{
	isWalking = true;

	isCrouching = false;
	isRunning = false;
}

void UCharacterAnimInstance::WalkReleased()
{
	isWalking = false;
	isCrouching = false;

	isRunning = true;
}

void UCharacterAnimInstance::CrouchPressed()
{
	if (!isCrouching)
	{
		isCrouching = true;

		isWalking = false;
		isRunning = false;
	}
	else
	{
		isCrouching = false;
		isWalking = false;

		isRunning = true;
	}
}

void UCharacterAnimInstance::UpdateMovementType()
{

}

void UCharacterAnimInstance::SetRunning()
{
	isRunning = true;

	isWalking = false;
	isCrouching = false;
}

void UCharacterAnimInstance::SetWalking()
{
	isWalking = true;

	isRunning = false;
	isCrouching = false;
}

void UCharacterAnimInstance::SetCrouching()
{
	isCrouching = true;

	isWalking = false;
	isRunning = false;
}

void UCharacterAnimInstance::JumpPressed()
{
	isJumping = true;
}

void UCharacterAnimInstance::WPress()
{
	//ClearWASD();

	if (isD)
		isWD = true;
	else if (isA)
		isWA = true;
	else
		isW = true;
}
void UCharacterAnimInstance::SPress()
{
	//ClearWASD();

	if (isD)
		isSD = true;
	else if (isA)
		isSA = true;
	else
		isS = true;
}
void UCharacterAnimInstance::DPress()
{
	//ClearWASD();

	if (isW)
		isWD = true;
	else if (isS)
		isSD = true;
	else
		isD = true;
}
void UCharacterAnimInstance::APress()
{
	//ClearWASD();

	if (isW)
		isWA = true;
	else if (isS)
		isSA = true;
	else
		isA = true;
}

void UCharacterAnimInstance::WRelease()
{
	if (isWD)
	{
		isWD = false;
		isD = true;
	}
	else if (isWA)
	{
		isWA = false;
		isA = true;
	}

	isW = false;
}
void UCharacterAnimInstance::SRelease()
{
	if (isSD)
	{
		isSD = false;
		isD = true;
	}
	else if (isSA)
	{
		isSA = false;
		isA = true;
	}

	isS = false;
}
void UCharacterAnimInstance::DRelease()
{
	if (isWD)
	{
		isWD = false;
		isW = true;
	}
	else if (isSD)
	{
		isSD = false;
		isS = true;
	}

	isD = false;
}
void UCharacterAnimInstance::ARelease()
{
	if (isWA)
	{
		isWA = false;
		isW = true;
	}
	else if (isSA)
	{
		isSA = false;
		isS = true;
	}

	isA = false;
}

void UCharacterAnimInstance::ClearWASD()
{
	isW = false;
	isS = false;
	isA = false;
	isD = false;
	isWA = false;
	isWD = false;
	isSA = false;
	isSD = false;
}

void UCharacterAnimInstance::JumpEndBeginNotyfyFromBP()
{
	AMainCharacter* owner = Cast<AMainCharacter>(GetOwningActor());
	if (owner)
	{
		owner->JumpEndBeginNotyfyCallback();
	}
}

void UCharacterAnimInstance::JumpEndEndNotyfyFromBP()
{
	AMainCharacter* owner = Cast<AMainCharacter>(GetOwningActor());
	if (owner)
	{
		owner->JumpEndEndNotyfyCallback();
	}
}

void UCharacterAnimInstance::StepNotyfyFromBP()
{
	FVector StartLocation = GetOwningActor()->GetActorLocation();
	FVector EndLocation = StartLocation - (GetOwningActor()->GetActorUpVector() * 100);

	FHitResult RaycastHit;

	FCollisionQueryParams CQP;
	CQP.bReturnPhysicalMaterial = true;
	CQP.AddIgnoredActor(GetOwningActor());

	GetWorld()->LineTraceSingleByChannel(RaycastHit, StartLocation, EndLocation, ECollisionChannel::ECC_WorldDynamic, CQP);


	if (!RaycastHit.bBlockingHit)
		return;

	USoundWave* sound = 0;
	int index = 0;

	switch (RaycastHit.PhysMaterial->SurfaceType)
	{
	case EPhysicalSurface::SurfaceType1:// carpet
		index = FMath::RandRange(0,CarpetSounds.Num()-1);
		sound = index<0? 0 : CarpetSounds[index];
		break;
	case EPhysicalSurface::SurfaceType2:// concrete
		index = FMath::RandRange(0, ConcreteSounds.Num()-1);
		sound = index < 0 ? 0 : ConcreteSounds[index];
		break;
	case EPhysicalSurface::SurfaceType3:// ground
		index = FMath::RandRange(0, GroundSounds.Num()-1);
		sound = index < 0 ? 0 : GroundSounds[index];
		break;
	case EPhysicalSurface::SurfaceType4:// metal
		index = FMath::RandRange(0, MetalSounds.Num()-1);
		sound = index < 0 ? 0 : MetalSounds[index];
		break;
	case EPhysicalSurface::SurfaceType5:// sand
		index = FMath::RandRange(0, SandSounds.Num()-1);
		sound = index < 0 ? 0 : SandSounds[index];
		break;
	case EPhysicalSurface::SurfaceType6:// water
		index = FMath::RandRange(0, WaterSounds.Num()-1);
		sound = index < 0 ? 0 : WaterSounds[index];
		break;
	case EPhysicalSurface::SurfaceType7:// wood
		index = FMath::RandRange(0, WoodSounds.Num()-1);
		sound = index < 0 ? 0 : WoodSounds[index];
		break;
	case EPhysicalSurface::SurfaceType10:// tile
		index = FMath::RandRange(0, TileSounds.Num()-1);
		sound = index < 0 ? 0 : TileSounds[index];
		break;
	default:
		sound = 0;
		break;
	}

	if (sound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetOwningActor()->GetActorLocation(),0.5f);
}