#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"

#include "HackScreenWidget.generated.h"

class UHackComponent;

UCLASS()
class VTM_API UHackScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHeader(TArray<FString>& lines);
	void SetHeader(FString line);
	void SetBody(FString s);
	void SetHelper(FString s);
	void SetBottom(FString s);
	void ClearInput();

	void SetHackComponent(UHackComponent* c) { hack_component = c; }

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	FEventReply OnKeyPress(FKeyEvent InKeyEvent);

	UFUNCTION(BlueprintCallable)
	void OnTextCommitted(const FText& text, ETextCommit::Type type);

	UFUNCTION(BlueprintCallable)
	void OnTextChanged(const FText& text);

private:
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Header;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Body;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Helper;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* Invite;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UEditableText* Input;

	UPROPERTY()
	UHackComponent* hack_component;
};
