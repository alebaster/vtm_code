#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"

#include "JournalWidget.generated.h"

class UButton;
class UScrollBox;

class UQuestEntryWidget;
class UQuestManager;

UCLASS()
class VTM_API UJournalWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetQuestManager(UQuestManager* qm) { quest_manager = qm; }

	void UpdatePage();

protected:
	virtual bool Initialize() override;

private:
	// 0 - santa, 1 - downtown, 2 - hollywood, 3 - chinatown
	int current_page = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UQuestEntryWidget> quest_entry_bp = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FSlateColor OriginalButtonColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FSlateColor ActiveButtonColor;

	UPROPERTY()
	UQuestManager* quest_manager = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* SMButton=0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SMText = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* DownButton=0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DownText = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* HollyButton=0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HollyText = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* ChinaButton=0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChinaText = 0;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ActiveScroll=0;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* CompletedScroll=0;
    
    UFUNCTION()
	void ClickSantaButton();
    
    UFUNCTION()
	void ClickDownButton();
    
    UFUNCTION()
	void ClickHollyButton();
    
    UFUNCTION()
	void ClickChinaButton();

	void PaintButtons();
};
