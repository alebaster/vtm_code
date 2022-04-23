#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DialogWidget.generated.h"

struct FDialogNodeStruct;
struct FDialogAnswerStruct;

class UDialogNodeWidget;
class UAnswerNodeWidget;

class UTextBlock;
class UScrollBox;

class UDialogManager;

UCLASS()
class VTM_API UDialogWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Show();
	void Hide();
	void Clear();

	//void AppendNode(FDialogNodeStruct* node, FString& name);
	void SetNode(FString& text);
	//void AppendAnswers(const TArray<FDialogAnswerStruct>* answers);
	void AppendAnswer(INT16 id, FString text);
	void ClearAnswers();
	//void FillAnswers(const TArray<FDialogAnswerStruct>* answers);

	void SetManager(UDialogManager* m) { manager = m; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables, meta = (BindWidget))
	UTextBlock* Node = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables, meta = (BindWidget))
	UVerticalBox* Answers = 0;
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	FEventReply OnKeyPress(FKeyEvent InKeyEvent);

private:
	UPROPERTY()
	UDialogManager* manager = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UAnswerNodeWidget> answer_node_widget_bp = 0;
};
