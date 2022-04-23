#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "Runtime/SlateCore/Public/Styling/SlateColor.h"

#include "AnswerNodeWidget.generated.h"

class UDialogWidget;

UCLASS()
class VTM_API UAnswerNodeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(FString text);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables, meta = (BindWidget), meta = (AllowPrivateAccess = "true"))
	UTextBlock* Text = 0;
};
