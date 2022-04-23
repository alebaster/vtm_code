#include "DialogableActorComponent.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "Core/RuntimeUtils.h"
#include "Dialog/DialogManager.h"
#include "Character/MainCharacter.h"

UDialogableActorComponent::UDialogableActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UDialogableActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

UTexture2D* UDialogableActorComponent::GetActionImage()
{
	return ActionImage;
}

int UDialogableActorComponent::Use(AActor* who)
{
	_who = who;

	USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	float d = URuntimeUtils::PlayMontage(m, intro_anim);

	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(montage_TimerHandle, this, &UDialogableActorComponent::IntroAnimationEnded, d, false);

	return 2;
}

void UDialogableActorComponent::EndUse(AActor* who)
{
	USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	float d = URuntimeUtils::PlayMontage(m, end_anim);
}

void UDialogableActorComponent::IntroAnimationEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(montage_TimerHandle);
	USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	URuntimeUtils::PlayMontage(m, dialog_anim,true);

	UDialogManager* dialog_manager = 0;

	AMainCharacter* character = Cast<AMainCharacter>(_who);
	if (character)
	{
		// move character camera to screen
		USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (m)
			character->MoveCameraTo(m->GetSocketTransform("LookSocket"));

		dialog_manager = Cast<UDialogManager>(character->GetComponentByClass(UDialogManager::StaticClass()));
	}
	if (!dialog_manager)
		UE_LOG(LogTemp, Error, TEXT("dialog_manager in UDialogableActorComponent::Use is null"))
	else
		dialog_manager->StartDialog(id);
}