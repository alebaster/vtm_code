#include "DamageWidget.h"

bool UDamageWidget::Initialize()
{
	Super::Initialize();

	if (!amount)
		UE_LOG(LogTemp, Error, TEXT("amount in UDamageWidget not binded"));

	return true;
}

void UDamageWidget::SetAmount(int num)
{
	amount->SetText(FText::AsNumber(num));
}

void UDamageWidget::Play()
{
	if (fade)
	{
		PlayAnimation(fade);
	}
}