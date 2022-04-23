#include "MainMultiWidget.h"

// GetWorld()
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

//#include "Character/MainCharacter.h"

bool UMainMultiWidget::Initialize()
{
	Super::Initialize();

	music = UGameplayStatics::SpawnSound2D(GetWorld(), BackgroundMusic, 1.0f, 1.0f, 0.0f, nullptr, false, false);
	if (!music)
		UE_LOG(LogTemp, Error, TEXT("music is null"))
	else
		music->Stop();

	init();
	
	if (SheetButton)
		SheetButton->OnClicked.AddDynamic(this, &UMainMultiWidget::ClickSheetButton);
	else
		UE_LOG(LogTemp, Error, TEXT("SheetButton not binded"))

	if (SheetButton)
		InfoButton->OnClicked.AddDynamic(this, &UMainMultiWidget::ClickInfoButton);
	else
		UE_LOG(LogTemp, Error, TEXT("InfoButton not binded"))

	if (SheetButton)
		QuestButton->OnClicked.AddDynamic(this, &UMainMultiWidget::ClickQuestButton);
	else
		UE_LOG(LogTemp, Error, TEXT("QuestButton not binded"));

	// get json sheet data
	FString path = FPaths::ProjectContentDir() + "RuntimeResources/etc/sheet.json";
	FString json_string;
	if (FFileHelper::LoadFileToString(json_string, *path))
		ParseJson(json_string);
	else
		UE_LOG(LogTemp, Error, TEXT("cannot open sheet.json"));

	return true;
}

// called every screen appearance
void UMainMultiWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMultiWidget::init()
{
	if (!Switcher)
	{
		UE_LOG(LogTemp, Error, TEXT("Switcher not binded"));
		return;
	}

	if (character_widget_bp)
		character_widget = WidgetTree->ConstructWidget<UCharacterWidget>(character_widget_bp);
	else
	{
		UE_LOG(LogTemp, Error, TEXT("character_widget_bp is null"));
		return;
	}

	if (!character_widget)
	{
		UE_LOG(LogTemp, Error, TEXT("character_widget is null"));
		return;
	}
	else
		character_widget->SetParentWidget(this);

	if (info_widget_bp)
		info_widget = WidgetTree->ConstructWidget<UInfoWidget>(info_widget_bp);
	else
	{
		UE_LOG(LogTemp, Error, TEXT("info_widget_bp is null"));
		return;
	}

	if (!info_widget)
	{
		UE_LOG(LogTemp, Error, TEXT("info_widget is null"));
		return;
	}

	if (journal_widget_bp)
	{
		journal_widget = WidgetTree->ConstructWidget<UJournalWidget>(journal_widget_bp);

		//AMainCharacter* owner = Cast<AMainCharacter>(GetOwner());
		//if (owner)
		//	quests_manager = Cast<UQuestManager>(owner->GetComponentByClass(UQuestManager::StaticClass()));
		//journal_widget->SetQuestManager();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("journal_widget_bp is null"));
		return;
	}

	if (!journal_widget)
	{
		UE_LOG(LogTemp, Error, TEXT("journal_widget is null"));
		return;
	}

	Switcher->AddChild(character_widget);
	Switcher->AddChild(info_widget);
	Switcher->AddChild(journal_widget);
}

void UMainMultiWidget::ClickSheetButton()
{
	if (!Switcher)
		return;

	if (Switcher->ActiveWidgetIndex == 0)
		return;

	ShowCharacter();
}

void UMainMultiWidget::ClickInfoButton()
{
	if (!Switcher)
		return;

	if (Switcher->ActiveWidgetIndex == 1)
		return;

	ShowInfo();
}

void UMainMultiWidget::ClickQuestButton()
{
	if (!Switcher)
		return;

	if (Switcher->ActiveWidgetIndex == 2)
		return;

	ShowJournal();
}

void UMainMultiWidget::ShowCharacter()
{
	if (!Switcher)
		return;

	if (!OnScreen())
	{
		PlayMusic();
		AddToViewport(0);
	}
	else if (Switcher->ActiveWidgetIndex == 0)
	{
		Hide();
		return;
	}

	if (SheetText && InfoText && QuestText)
	{
		SheetText->SetColorAndOpacity(ActiveButtonColor);
		InfoText->SetColorAndOpacity(OriginalButtonColor);
		QuestText->SetColorAndOpacity(OriginalButtonColor);
	}

	Switcher->SetActiveWidgetIndex(0);
}

void UMainMultiWidget::ShowInfo()
{
	if (!Switcher)
		return;

	if (!OnScreen())
	{
		PlayMusic();
		AddToViewport(0);
	}
	else if (Switcher->ActiveWidgetIndex == 1)
	{
		Hide();
		return;
	}

	if (SheetText && InfoText && QuestText)
	{
		SheetText->SetColorAndOpacity(OriginalButtonColor);
		InfoText->SetColorAndOpacity(ActiveButtonColor);
		QuestText->SetColorAndOpacity(OriginalButtonColor);
	}

	Switcher->SetActiveWidgetIndex(1);
}

void UMainMultiWidget::ShowJournal()
{
	if (!Switcher)
		return;

	if (!OnScreen())
	{
		PlayMusic();
		AddToViewport(0);
	}
	else if (Switcher->ActiveWidgetIndex == 2)
	{
		Hide();
		return;
	}

	if (SheetText && InfoText && QuestText)
	{
		SheetText->SetColorAndOpacity(OriginalButtonColor);
		InfoText->SetColorAndOpacity(OriginalButtonColor);
		QuestText->SetColorAndOpacity(ActiveButtonColor);
	}

	if (journal_widget)
		journal_widget->UpdatePage();

	Switcher->SetActiveWidgetIndex(2);
}

void UMainMultiWidget::Hide()
{
	RemoveFromViewport();

	StopMusic();
}

void UMainMultiWidget::UpdateHoverCharacter(FString key1, FString key2)
{
	if (!character_widget)
		return;

	FSheetDescr* sd1 = 0;
	FSheetDescr* sd2 = 0;

	FString title1("");
	FString text1("");
	FString title2("");
	FString text2("");

	if (sheet_data.Contains(key1))
	{
		sd1 = &sheet_data[key1];
		title1 = sd1->display_name;
		text1 = sd1->description;
	}

	if (sheet_data.Contains(key2))
	{
		sd2 = &sheet_data[key2];
		title2 = sd2->display_name;
		text2 = sd2->description;
	}

	character_widget->SetInfo(title1, text1, title2, text2);

	//if (sd1)
	//	character_widget->SetInfo1(sd1->display_name, sd1->description);
	//
	//if (sd2)
	//	character_widget->SetInfo2(sd2->display_name, sd2->description);
}

void UMainMultiWidget::PlayMusic()
{
	//if (!music)
	//	music = UGameplayStatics::SpawnSound2D(GetWorld(), BackgroundMusic, 1.0f, 1.0f, 0.0f, nullptr, false, false);

	if (!music)
		return;
	
	music->Play();
	//UGameplayStatics::PlaySound2D(GetWorld(), BackgroundMusic);
}

void UMainMultiWidget::StopMusic()
{
	if (!music)
		return;

	music->Stop();
}

void UMainMultiWidget::ParseJson(FString& string)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(string);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("json sheet file is not valid"));
		return;
	}

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FillSheetStruct(JsonObject);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("cannot decerialize json sheet file"));
}

void UMainMultiWidget::FillSheetStruct(TSharedPtr<FJsonObject> jobj)
{
	get_sections(jobj);
	get_attributes(jobj);
	get_feats(jobj);
}

void UMainMultiWidget::get_sections(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> objArray = jobj->GetArrayField(TEXT("Sections"));
	for (int32 i = 0; i < objArray.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FSheetDescr sd;

		sd.id = object->GetStringField(TEXT("id"));
		sd.display_name = object->GetStringField(TEXT("display_name"));
		sd.description = object->GetStringField(TEXT("description"));

		sheet_data.Add(sd.id, sd);
	}
}

void UMainMultiWidget::get_attributes(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> objArray = jobj->GetArrayField(TEXT("Attributes"));
	for (int32 i = 0; i < objArray.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FSheetDescr sd;

		sd.id = object->GetStringField(TEXT("id"));
		sd.display_name = object->GetStringField(TEXT("display_name"));
		sd.description = object->GetStringField(TEXT("description"));
		get_links("affect",sd.links,object);

		sheet_data.Add(sd.id, sd);
	}
}

void UMainMultiWidget::get_feats(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> objArray = jobj->GetArrayField(TEXT("Feats"));
	for (int32 i = 0; i < objArray.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FSheetDescr sd;

		sd.id = object->GetStringField(TEXT("id"));
		sd.display_name = object->GetStringField(TEXT("display_name"));
		sd.description = object->GetStringField(TEXT("description"));
		get_links("dependence", sd.links, object);

		sheet_data.Add(sd.id, sd);
	}
}

void UMainMultiWidget::get_links(FString name, TArray<FString>& links, TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> ja;
	const TArray<TSharedPtr<FJsonValue>>* json_array = &ja;
	if (!json_array)
		return;

	jobj->TryGetArrayField(name, json_array);
	for (int32 i = 0; i < json_array->Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = (*json_array)[i];
		FString link = value->AsString();

		links.Add(link);
	}
}