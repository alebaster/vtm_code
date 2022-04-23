#include "QuestEntryWidget.h"

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

bool UQuestEntryWidget::Initialize()
{
	Super::Initialize();

	if (!Title)
		UE_LOG(LogTemp, Error, TEXT("Title in QuestEntryWidget not binded"))

	if (!Body)
		UE_LOG(LogTemp, Error, TEXT("Body in QuestEntryWidget not binded"))

	return true;
}

void UQuestEntryWidget::SetTitle(const FString& s)
{
    if (!Title)
        return;
    
    Title->SetText(FText::FromString(s));
}

void UQuestEntryWidget::SetBody(const FString& s)
{
    if (!Body)
        return;
    
    Body->SetText(FText::FromString(s));
}