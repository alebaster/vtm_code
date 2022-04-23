#include "DialogWidget.h"

#include "UI/AnswerNodeWidget.h"
#include "UI/DialogNodeWidget.h"

#include "Dialog/DialogManager.h"

void UDialogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
}

void UDialogWidget::Show()
{
	AddToViewport(0);
}

void UDialogWidget::Hide()
{
	RemoveFromViewport();
}

void UDialogWidget::Clear()
{
	if (Answers)
		Answers->ClearChildren();
}

void UDialogWidget::ClearAnswers()
{
	if (Answers)
		Answers->ClearChildren();
}

void UDialogWidget::SetNode(FString& text)
{
	if (!Node)
		return;

	Node->SetText(FText::FromString(text));
}

void UDialogWidget::AppendAnswer(INT16 id, FString text)
{
	if (!Answers || !answer_node_widget_bp)
		return;

	UAnswerNodeWidget* answer_widget = WidgetTree->ConstructWidget<UAnswerNodeWidget>(answer_node_widget_bp);
	answer_widget->SetText(text);

	Answers->AddChild(answer_widget);
	UE_LOG(LogTemp, Warning, TEXT("add answer: %s "), *text);
	//TextEdit->SetKeyboardFocus();
	SetKeyboardFocus();
}

FEventReply UDialogWidget::OnKeyPress(FKeyEvent InKeyEvent)
{
	return FEventReply(false);

	UE_LOG(LogTemp, Warning, TEXT("UDialogWidget::OnKeyPress"))

	INT16 n = 0;

	FKey k = InKeyEvent.GetKey();

	if (manager)
	{
		if (k == "One")
			n = 1;
		else if (k == "Two")
			n = 2;
		else if (k == "Three")
			n = 3;
		else if (k == "Four")
			n = 4;
		else if (k == "Five")
			n = 5;
		else if (k == "Six")
			n = 6;
		else if (k == "Seven")
			n = 7;
		else if (k == "Eight")
			n = 8;
		else if (k == "Nine")
			n = 9;

		manager->ReactToAnswer(n);
	}

	return FEventReply(false);
}