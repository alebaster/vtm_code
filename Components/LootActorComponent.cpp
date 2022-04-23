#include "LootActorComponent.h"

#include "VtmPlayerController.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/LootBoxAnimInstance.h"

ULootActorComponent::ULootActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void ULootActorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

int ULootActorComponent::Use(AActor* who)
{
    AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!controller)
    {
		UE_LOG(LogTemp, Error, TEXT("VtmPlayerController in LootActorComponent is null"));
        return 0;
    }
    
    SetAnimOpen();
    controller->StartBarter(loot);
    
    return 5;
}

void ULootActorComponent::EndUse(AActor* who)
{
    SetAnimClose();
}

UTexture2D* ULootActorComponent::GetActionImage()
{
    return ActionImage;
}

void ULootActorComponent::SetAnimOpen()
{
    USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
    if (m)
    {
        ULootBoxAnimInstance* anim_inst = Cast<ULootBoxAnimInstance>(m->GetAnimInstance());
        if (anim_inst)
            anim_inst->isUse = true;
		else
			UE_LOG(LogTemp, Error, TEXT("anim_inst in ULootActorComponent is null"));
    }
	else
		UE_LOG(LogTemp, Error, TEXT("USkeletalMeshComponent in ULootActorComponent is null"));
}

void ULootActorComponent::SetAnimClose()
{
    USkeletalMeshComponent* m = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
    if (m)
    {
        ULootBoxAnimInstance* anim_inst = Cast<ULootBoxAnimInstance>(m->GetAnimInstance());
        if (anim_inst)
            anim_inst->isUse = false;
    }
}

