#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"
#include "Runtime/UMG/Public/Components/CheckBox.h"

#include "Runtime/Core/Public/Containers/Map.h"

#include <map>
#include <vector>

#include "CharacterWidget.generated.h"

class UMainMultiWidget;

UCLASS()
class VTM_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetParentWidget(UMainMultiWidget* pw) { parent_widget = pw; }
	void SetInfo(FString& title1, FString& text1, FString& title2, FString& text2);
	void SetInfo1(FString& title1, FString& text1);
	void SetInfo2(FString& title2, FString& text2);
	void Hightlight(TArray <FString>& keys);

protected:
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

private:
	UPROPERTY()
	UMainMultiWidget* parent_widget;

	UPROPERTY()
	TMap<FString,UButton*> hover_buttons;

	//UPROPERTY()
	//TMap<FString, TArray<UCheckBox*>> bubbles;
	std::map<FString, std::vector<UCheckBox*>> bubbles;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title_1 = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_1 = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Title_2 = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_2 = 0;

	UFUNCTION()
	void button_hover();

	UFUNCTION()
	void button_unhover();

	UFUNCTION()
	void bubble_changed(bool on);
};
