#include "JournalWidget.h"

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

#include "Quest/QuestManager.h"
#include "UI/QuestEntryWidget.h"

bool UJournalWidget::Initialize()
{
	Super::Initialize();

	if (!quest_entry_bp)
		UE_LOG(LogTemp, Error, TEXT("quest_entry_bp in JournalWidget is null"))

    if (SMButton)
		SMButton->OnClicked.AddDynamic(this, &UJournalWidget::ClickSantaButton);
    else
		UE_LOG(LogTemp, Error, TEXT("SMButton in JournalWidget not binded"))
    
    if (DownButton)
		DownButton->OnClicked.AddDynamic(this, &UJournalWidget::ClickDownButton);
    else
		UE_LOG(LogTemp, Error, TEXT("DownButton in JournalWidget not binded"))
    
    if (HollyButton)
		HollyButton->OnClicked.AddDynamic(this, &UJournalWidget::ClickHollyButton);
    else
		UE_LOG(LogTemp, Error, TEXT("HollyButton in JournalWidget not binded"))

    if (ChinaButton)
		ChinaButton->OnClicked.AddDynamic(this, &UJournalWidget::ClickChinaButton);
    else
		UE_LOG(LogTemp, Error, TEXT("ChinaButton in JournalWidget not binded"))

    if (!ActiveScroll)
        UE_LOG(LogTemp, Error, TEXT("ActiveScroll in JournalWidget not binded"))

    if (!CompletedScroll)
        UE_LOG(LogTemp, Error, TEXT("CompletedScroll in JournalWidget not binded"))
    
	return true;
}

void UJournalWidget::UpdatePage()
{
    if (!quest_manager)
    {
        UE_LOG(LogTemp, Error, TEXT("quest manager in JournalWidget is not set"))
        return;
    }
    
    if (!quest_entry_bp)
    {
        UE_LOG(LogTemp, Error, TEXT("quest_entry_bp in JournalWidget is not set"))
        return;
    }
    
    if (ActiveScroll)
        ActiveScroll->ClearChildren();
    
    if (CompletedScroll)
        CompletedScroll->ClearChildren();

    const TArray<FActiveQuestStruct>& quests = quest_manager->GetActiveQuests();
    //UE_LOG(LogTemp, Warning, TEXT("num of active quests: %d"), quests.Num());

    TArray<UQuestEntryWidget*> active_widgets;
	TArray<UQuestEntryWidget*> finished_widgets;
    for (auto& q : quests)
    {
        //UE_LOG(LogTemp, Warning, TEXT("GetQuestArea: %d"), quest_manager->GetQuestArea(q.id));
        //UE_LOG(LogTemp, Warning, TEXT("current_page: %d"), current_page);
        
        if (quest_manager->GetQuestArea(q.id) != current_page)
            continue;
        //UE_LOG(LogTemp, Warning, TEXT("search: %s %d"), *q.id, q.stage_num)

        const FQuestStageStruct* stage = quest_manager->GetQuestStage(q.id,q.stage_num);
        if (!stage)
            continue;
        //UE_LOG(LogTemp, Warning, TEXT("find stage: %s"), *stage->descr)
        UQuestEntryWidget* quest_widget = WidgetTree->ConstructWidget<UQuestEntryWidget>(quest_entry_bp);
        //quest_widget->SetParent(this);
        quest_widget->SetTitle(quest_manager->GetQuestName(q.id));
        quest_widget->SetBody(stage->descr);
        if (stage->type == 0) // active
            active_widgets.Insert(quest_widget,0);
        else if (stage->type == 1) // finished
            finished_widgets.Insert(quest_widget,0);
    }
    
    for (auto& w : active_widgets)
        ActiveScroll->AddChild(w);
    
    for (auto& w : finished_widgets)
        CompletedScroll->AddChild(w);

    PaintButtons();
}

void UJournalWidget::ClickSantaButton()
{
    if (current_page != 0)
    {
        current_page = 0;
        UpdatePage();
    }
}

void UJournalWidget::ClickDownButton()
{
    if (current_page != 1)
    {
        current_page = 1;
        UpdatePage();
    }
}

void UJournalWidget::ClickHollyButton()
{
    if (current_page != 2)
    {
        current_page = 2;
        UpdatePage();
    }
}

void UJournalWidget::ClickChinaButton()
{
    if (current_page != 3)
    {
        current_page = 3;
        UpdatePage();
    }
}

void UJournalWidget::PaintButtons()
{
    SMText->SetColorAndOpacity(OriginalButtonColor);
    DownText->SetColorAndOpacity(OriginalButtonColor);
    HollyText->SetColorAndOpacity(OriginalButtonColor);
    ChinaText->SetColorAndOpacity(OriginalButtonColor);

    if (current_page == 0)
        SMText->SetColorAndOpacity(ActiveButtonColor);
    else if (current_page == 1)
        DownText->SetColorAndOpacity(ActiveButtonColor);
    else if (current_page == 2)
        HollyText->SetColorAndOpacity(ActiveButtonColor);
    else if (current_page == 3)
        ChinaText->SetColorAndOpacity(ActiveButtonColor);
}