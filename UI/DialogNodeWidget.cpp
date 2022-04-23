#include "DialogNodeWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

void UDialogNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ttt->OnClicked.AddDynamic(this, &UDialogWidget::Show);
	//if (Person)
	//	Person->SetText(FText::FromString(TEXT("666")));
}

void UDialogNodeWidget::SetName(const FString& text)
{
	if (Name)
		Name->SetText(FText::FromString(text));
}

void UDialogNodeWidget::SetText(const FString& text)
{
	UE_LOG(LogTemp, Warning, TEXT("set dialog node text %s"), *text);

	if (Text)
		Text->SetText(FText::FromString(text));
}

