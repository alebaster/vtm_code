#include "WodComponent.h"

#include "Character/MainCharacter.h"
#include "UI/MainHUD.h"

UWodComponent::UWodComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWodComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

int UWodComponent::GetHP()
{
	return hp;
}

int UWodComponent::ChangeHP(int amount)
{
	hp += amount;

	if (hp < 0)
		hp = 0;
	else if (hp > 100)
		hp = 100;

	return hp;
}

void UWodComponent::GiveExp(int amount)
{
	AMainCharacter* owner = Cast<AMainCharacter>(GetOwner());
	if (owner)
	{
		UMainHUD* hud = owner->GetHud();
		if (hud)
			hud->ShowExp(amount);
	}
}
