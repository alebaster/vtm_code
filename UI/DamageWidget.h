#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DamageWidget.generated.h"

UCLASS()
class VTM_API UDamageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetAmount(int num);
	void Play();

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* amount = 0;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* fade=0;
};
