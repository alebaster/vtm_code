#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

class UTextBlock;

#include "DialogNodeWidget.generated.h"

UCLASS()
class VTM_API UDialogNodeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetName(const FString& text);

	UFUNCTION()
	void SetText(const FString& text);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	UTextBlock* Name = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	UTextBlock* Text = 0;
	
};
