#include "PickableActorComponent.h"

#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Lua/LuaComponent.h"

UPickableActorComponent::UPickableActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickableActorComponent::BeginPlay()
{
	Super::BeginPlay();

}

int UPickableActorComponent::Use(AActor* who)
{
    UInventoryComponent* inventory_manager = 0;
    
    AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
		inventory_manager = Cast<UInventoryComponent>(controller->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (!inventory_manager)
    {
		UE_LOG(LogTemp, Error, TEXT("inventory_manager in UPickableActorComponent is null"));
        return 0;
    }
    
    inventory_manager->AddItem(item_id,amount);
    
    // destroy parent
    AActor* a = GetOwner();
    if (a)
    {
        a->SetActorHiddenInGame(true);
        a->Destroy();
    }
    
    if (!lua_script.IsEmpty())
    {
        AMainCharacter* character = Cast<AMainCharacter>(who);
        if (character)
        {
            ULuaComponent* lua = Cast<ULuaComponent>(character->GetComponentByClass(ULuaComponent::StaticClass()));
            if (!lua)
                UE_LOG(LogTemp, Error, TEXT("cannot get lua_component from owner"))
            else
                lua->ExecuteStringWOResult(lua_script);
        }
    }

	return 0;
}

UTexture2D* UPickableActorComponent::GetActionImage()
{
    return ActionImage;
}


