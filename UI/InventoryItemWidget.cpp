#include "InventoryItemWidget.h"

#include "UI/InventoryWidget.h"

bool UInventoryItemWidget::Initialize()
{
	Super::Initialize();

	if (Button)
		Button->OnClicked.AddDynamic(this, &UInventoryItemWidget::ClickButton);
	else
		UE_LOG(LogTemp, Error, TEXT("UInventoryItemWidget Button not binded"))

	if (!BackImage)
		UE_LOG(LogTemp, Error, TEXT("UInventoryItemWidget BackImage is NULL"))

	if (!BackActiveImage)
		UE_LOG(LogTemp, Error, TEXT("UInventoryItemWidget BackActiveImage is NULL"))

	return true;
}

void UInventoryItemWidget::SetFore(UTexture2D* image)
{
	if (Fore)
		Fore->SetBrushFromTexture(image);

	if (Back)
		Back->SetBrushFromTexture(BackImage);
}

void UInventoryItemWidget::SetActive()
{
	if (Back)
		Back->SetBrushFromTexture(BackActiveImage);

	if (Button)
		Button->SetColorAndOpacity(ActiveButtonColor);
}

void UInventoryItemWidget::SetPassive()
{
	if (Back)
		Back->SetBrushFromTexture(BackActiveImage);

	if (Button)
		Button->SetColorAndOpacity(OriginalButtonColor);
}

void UInventoryItemWidget::SetCounter(int n)
{
    if (!Counter)
        return;
    
    FString s("");
    if (n!=1)
        s = FString::FromInt(n);
    
    //Counter->SetText(FText::FromString(FString::FromInt(one)+"/"+FString::FromInt(two)));
    Counter->SetText(FText::FromString(s));
}

void UInventoryItemWidget::ClickButton()
{
	if (parent_widget)
		parent_widget->ClickItem(this);
}
