#include "UsableWidgetComponent.h"
#include "Blueprint/UserWidget.h"

#include "Character/MainCharacter.h"
#include "Lua/LuaComponent.h"

UUsableWidgetComponent::UUsableWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUsableWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

UTexture2D* UUsableWidgetComponent::GetActionImage()
{
	return ActionImage;
}

int UUsableWidgetComponent::Use(AActor* who)
{
	if (!widget)
		return 0;

	if (!widget->IsInViewport())
	{
		widget->AddToViewport(0);

		if (!lua_script.IsEmpty())
		{
			AMainCharacter* character = Cast<AMainCharacter>(who);
			if (character)
			{
				lua = Cast<ULuaComponent>(character->GetComponentByClass(ULuaComponent::StaticClass()));
				if (!lua)
					UE_LOG(LogTemp, Error, TEXT("cannot get lua_component from owner"))
				else
					lua->ExecuteString(lua_script);
			}
		}
	}
    
    return 1;
}

void UUsableWidgetComponent::EndUse(AActor* who)
{
    (void)who;
    
    if (!widget)
		return;

	if (widget->IsInViewport())
        widget->RemoveFromViewport();
}