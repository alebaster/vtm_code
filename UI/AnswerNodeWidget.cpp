#include "AnswerNodeWidget.h"

#include "UI/DialogWidget.h"
#include "Dialog/DialogManager.h"

void UAnswerNodeWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAnswerNodeWidget::SetText(FString text)
{
	if (Text)
		Text->SetText(FText::FromString(text));
}
