#include "CharacterWidget.h"

//#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"

#include "UI/MainMultiWidget.h"

bool UCharacterWidget::Initialize()
{
	Super::Initialize();

	//TArray <UUserWidget*> FoundWidgets;
	//UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), FoundWidgets, UButton::StaticClass(), false);
	//UE_LOG(LogTemp, Error, TEXT("buttons: %d"), FoundWidgets.Num());

	if (!Title_1 || !Title_2 || !Text_1 || !Text_2)
		UE_LOG(LogTemp, Error, TEXT("info block bind problem"))
	else
	{
		Title_1->SetText(FText::FromString(""));
		Text_1->SetText(FText::FromString(""));
		Title_2->SetText(FText::FromString(""));
		Text_2->SetText(FText::FromString(""));
	}

	TArray <UWidget*> all_widgets;
	WidgetTree->GetAllWidgets(all_widgets);
	for (auto widget : all_widgets)
	{
		UButton* button = Cast<UButton>(widget);
		if (button)
		{
			FString name;
			button->GetName().Split(TEXT("_"),&name,0);
			hover_buttons.Add(name, button);

			button->OnHovered.AddDynamic(this, &UCharacterWidget::button_hover);

			continue;
		}

		UCheckBox* box = Cast<UCheckBox>(widget);
		if (box)
		{
			FString parent_name = box->GetParent()->GetName();
			bubbles[parent_name].push_back(box);

			box->OnCheckStateChanged.AddDynamic(this,&UCharacterWidget::bubble_changed);
			//if (bubbles.Contains(parent_name))
			//{
				//bubbles[parent_name].Add(box);
			//}
		}
	}

	if (!bubbles.size())
		UE_LOG(LogTemp, Error, TEXT("charsheet widget bubbles not found"));

	if (!hover_buttons.Num())
		UE_LOG(LogTemp, Error, TEXT("charsheet widget hover buttons not found"));
	
	return true;
}

// called every screen appearance
void UCharacterWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCharacterWidget::SetInfo(FString& title1, FString& text1, FString& title2, FString& text2)
{
	if (!Title_1 || !Text_1 || !Title_2 || !Text_2)
		return;

	if (title1 == "")
	{
		Title_1->SetText(FText::FromString(title2));
		Text_1->SetText(FText::FromString(text2));
		Title_2->SetText(FText::FromString(""));
		Text_2->SetText(FText::FromString(""));
	}
	else
	{
		Title_1->SetText(FText::FromString(title1));
		Text_1->SetText(FText::FromString(text1));
		Title_2->SetText(FText::FromString(title2));
		Text_2->SetText(FText::FromString(text2));
	}
}

void UCharacterWidget::SetInfo1(FString& title1, FString& text1)
{
	if (Title_1)
		Title_1->SetText(FText::FromString(title1));

	if (Text_1)
		Text_1->SetText(FText::FromString(text1));
}

void UCharacterWidget::SetInfo2(FString& title2, FString& text2)
{
	if (Title_2)
		Title_2->SetText(FText::FromString(title2));

	if (Text_2)
		Text_2->SetText(FText::FromString(text2));
}

void UCharacterWidget::Hightlight(TArray <FString>& keys)
{

}

void UCharacterWidget::button_hover()
{
	for (auto widget : hover_buttons)
	{
		if (widget.Value->IsHovered())
		{
			//UE_LOG(LogTemp, Error, TEXT("hover %s"), *widget.Key);
			if (parent_widget)
			{
				FString p_name = widget.Value->GetParent()->GetName();
				parent_widget->UpdateHoverCharacter(p_name,widget.Key);
			}
			break;
		}
	}
}

void UCharacterWidget::button_unhover()
{
	if (parent_widget)
		parent_widget->UpdateHoverCharacter(FString(""), FString(""));
}

void UCharacterWidget::bubble_changed(bool on)
{
	for (auto pair : bubbles)
	{
		for (int i=0;i<pair.second.size();++i)
		{
			if (pair.second[i]->IsHovered())
			{
				//UE_LOG(LogTemp, Error, TEXT("PUK %d"), i);
				break;
			}
			
		}
	}
}