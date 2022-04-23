#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"



#include "QuestEntryWidget.generated.h"

class UTextBlock;

UCLASS()
class VTM_API UQuestEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetTitle(const FString& s);
    void SetBody(const FString& s);
    
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Body = 0;
};
