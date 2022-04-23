#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// umg
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"
#include "Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"
#include "Runtime/UMG/Public/Components/VerticalBox.h"

#include "InventoryItemWidget.generated.h"

class UInventoryWidget;

UCLASS()
class VTM_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetParent(UInventoryWidget* w) { parent_widget = w; }

	UPROPERTY(EditAnywhere)
	UTexture2D* BackImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* BackActiveImage;

	void SetFore(UTexture2D* image);
	void SetActive();
	void SetPassive();
    void SetCounter(int n);
    //void HideCounter();

	FString id;
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor OriginalButtonColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor ActiveButtonColor;

	UPROPERTY(meta = (BindWidget))
	UButton* Button = 0;

	UPROPERTY(meta = (BindWidget))
	UImage* Back = 0;

	UPROPERTY(meta = (BindWidget))
	UImage* Fore = 0;
    
    UPROPERTY(meta = (BindWidget))
	UTextBlock* Counter = 0;

	UFUNCTION()
	void ClickButton();

	UInventoryWidget* parent_widget;
};
