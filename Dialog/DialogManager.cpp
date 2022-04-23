#include "DialogManager.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "TimerManager.h"

//#include "Runtime/Core/Public/Misc/FileHelper.h"
//#include "Runtime/Core/Public/Misc/Paths.h"
//#include "Runtime/Json/Public/Dom/JsonObject.h"
//#include "Runtime/Json/Public/Serialization/JsonReader.h"
//#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "Core/RuntimeUtils.h"
#include "VtmPlayerController.h"
#include "Actors/DialogableActorComponent.h"
#include "UI/DialogWidget.h"
#include "Lua/LuaComponent.h"

UDialogManager::UDialogManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDialogManager::BeginPlay()
{
	Super::BeginPlay();
	
	FindCurrentLevelDialogs();
    
    if (dialog_widget_bp)
	{
		AController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        APlayerController* PlayerController = Cast<APlayerController>(controller);
		if (PlayerController)
		{
			dialog_widget = CreateWidget<UDialogWidget>(PlayerController, dialog_widget_bp);
			dialog_widget->SetManager(this);
		}
	}
    else
        UE_LOG(LogTemp, Error, TEXT("dialog_widget_bp in dialog manager is null"));
}

void UDialogManager::StartDialog(FString& id)
{
    if (!dialog_widget)
	{
		UE_LOG(LogTemp, Error, TEXT("cant start dialog with null dialog widget"));
		return;
	}

	FDialog* dialog = GetDialog(id);
	if (!dialog)
	{
		UE_LOG(LogTemp, Error, TEXT("cant find dialog with id: %s"),*id);
		return;
	}
	if (!dialog->nodes.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("dialog with id: %s have no nodes"),*id);
		return;
	}
	if (!dialog->answers.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("dialog with id: %s have no answers"),*id);
		return;
	}

	INT16 start_id=0;
	if (lua_component)
	{
		for (auto& s : dialog->starts)
		{
			if (lua_component->ExecuteStringWBoolResult(s.condition))
			{
				start_id = s.id;
				//UE_LOG(LogTemp, Warning, TEXT("dialog start id %d with condition %s"), start_id, *s.condition);
				break;
			}
		}
	}
	if (start_id == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("all start conditions for dialog: %s are failed"),*id);
		start_id = 1;
	}

	FillDialogWidget(dialog,start_id);

	dialog_widget->Show();
}

void UDialogManager::FillDialogWidget(FDialog* dialog, INT16 node_id)
{
	current_answers.Empty();

	FDialogNode* start_node = 0;
	auto node_found = dialog->nodes.Find(node_id);
	if (node_found)
		start_node = node_found->Get();
	if (!start_node)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot find node: %d in dialog: %s "), node_id, *dialog->id);
		return; // add true return
	}

	current_dialog_id = dialog->id;
	current_node_id = node_id;

	dialog_widget->Clear();//? better inside widget
	dialog_widget->SetNode(start_node->text);

	//line1_col_e
	FString name = "line" + FString::FromInt(current_node_id) + "_col_e";
	FString path = FPaths::ProjectContentDir() + "RuntimeResources/dialog_sound/" + current_dialog_id + "/" + name + ".wav";
	USoundWave* wav = URuntimeUtils::GetSoundWave_FromFile(path);
	if (!wav)
		UE_LOG(LogTemp, Error, TEXT("cannot load wav file: %s "), node_id)
	else
	{
		current_wav = UGameplayStatics::CreateSound2D(GetWorld(), wav);
		current_wav->Play();
		//UGameplayStatics::PlaySound2D(GetWorld(), wav);

		//FTimerDelegate TimerDel;
		//TimerDel.BindUFunction(this, FName("WavEnded"), dialog, start_node);

		GetWorld()->GetTimerManager().ClearTimer(wav_TimerHandle);
		GetWorld()->GetTimerManager().SetTimer(wav_TimerHandle, this, &UDialogManager::WavEnded, wav->Duration, false);
		UE_LOG(LogTemp, Error, TEXT("line duration: %f "), wav->Duration)
	}

	if (lua_component)
		lua_component->ExecuteStringWOResult(start_node->effect);
}

void UDialogManager::WavEnded()
{
	GetWorld()->GetTimerManager().ClearTimer(wav_TimerHandle);

	FDialog* dialog = GetDialog(current_dialog_id);
	if (!dialog)
		return;
	FDialogNode* start_node = 0;
	auto node_found = dialog->nodes.Find(current_node_id);
	if (node_found)
		start_node = node_found->Get();
	if (!start_node)
		return;

	FDialogAnswer* answer = 0;
	int num = 1;
	for (int answer_id : start_node->answers)
	{
		auto answer_found = dialog->answers.Find(answer_id);
		if (answer_found)
		{
			answer = answer_found->Get();
			if (lua_component)
			{
				if (lua_component->ExecuteStringWBoolResult(answer->condition))
				{
					if (answer->text == "(Auto-Link)")
					{
						FillDialogWidget(dialog, (*answer_found)->next_node);
						return;
					}
					dialog_widget->AppendAnswer(answer->id, FString::FromInt(num) + FString(". ") + answer->text);
					current_answers.Add(num, answer->id);
					num++;
				}
			}
		}
	}
}

void UDialogManager::Skip()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(wav_TimerHandle))
	{
		current_wav->Stop();
		GetWorld()->GetTimerManager().ClearTimer(wav_TimerHandle);
	}
	else
		return;

	WavEnded();
}

void UDialogManager::ReactToAnswer(INT16 key)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(wav_TimerHandle))
		return;

	GetWorld()->GetTimerManager().ClearTimer(wav_TimerHandle);

	INT16 answer_id=0;
	auto f = current_answers.Find(key);
	if (f)
		answer_id = *f;

	if (answer_id == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("cannot find answer with key %d and id: %d "), key, answer_id);
		EndDialog();
		return;
	}

	FDialog* dialog = GetDialog(current_dialog_id);
	if (dialog)
	{
		auto answer_found = dialog->answers.Find(answer_id);
		if (answer_found)
		{
			// true exit from dialog
			if ((*answer_found)->next_node == 0)
				EndDialog();
			else 
				FillDialogWidget(dialog, (*answer_found)->next_node);
		}
	}
}

void UDialogManager::EndDialog()
{
	UE_LOG(LogTemp, Error, TEXT("EndDialog"));

	AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
	{
		controller->EndUse();
		dialog_widget->Clear();
		dialog_widget->Hide();
	}
}

FDialog* UDialogManager::GetDialog(FString& id)
{  
    auto ret = dialogs.Find(id);
	
	return ret != 0 ? ret->Get() : 0;
}

void UDialogManager::FindCurrentLevelDialogs()
{
	TArray<AActor*> all_actors;
	UGameplayStatics::GetAllActorsOfClass((UObject*)GetWorld(), ACharacter::StaticClass(), all_actors);

	if (!all_actors.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("no characters in dialog manager"));
		return;
	}

	for (AActor* a : all_actors)
	{
		UDialogableActorComponent* d = Cast<UDialogableActorComponent>(a->GetComponentByClass(UDialogableActorComponent::StaticClass()));;
		if (d)
		{
			ParseDialog(d->id);
			UE_LOG(LogTemp, Warning, TEXT("find dialog in actor with name: %s"),*a->GetName());
		}
	}
}

void UDialogManager::ParseDialog(FString id)
{
	FString path = FPaths::ProjectContentDir() + "RuntimeResources/dialog/"+id+".json";
	FString json_string;
	if (FFileHelper::LoadFileToString(json_string, *path))
		ParseJson(json_string, id);
	else
		UE_LOG(LogTemp, Error, TEXT("cannot open dialog file with id %s"),*id);
}

// json processing
void UDialogManager::ParseJson(FString& string, FString& id)
{
    TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(string);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("json dialog with id %s is not valid"), *id);
		return;
	}

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		FillStructs(JsonObject, id);
	else
		UE_LOG(LogTemp, Error, TEXT("cannot deserialize json dialog with id %s"), *id);
}

void UDialogManager::FillStructs(TSharedPtr<FJsonObject> jobj, FString& id)
{
    smartFDialog dialog(new FDialog());
    
	get_dialog_starts(dialog->starts, jobj);
    get_dialog_nodes(dialog->nodes, jobj);
    get_dialog_answers(dialog->answers, jobj);
	dialog->id = id;
    
    dialogs.Add(id, dialog);
}

void UDialogManager::get_dialog_nodes(TMap<INT16, smartFDialogNode>& nodes, TSharedPtr<FJsonObject>& jobj)
{
    TArray<TSharedPtr<FJsonValue>> nodes_json = jobj->GetArrayField(TEXT("nodes"));
	for (int32 i=0; i<nodes_json.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = nodes_json[i];
		TSharedPtr<FJsonObject> node_obj = value->AsObject();
        
        smartFDialogNode node(new FDialogNode());

		node->id = node_obj->GetIntegerField(TEXT("id"));
		node->text = node_obj->GetStringField(TEXT("text"));
		node->effect = node_obj->GetStringField(TEXT("effect"));
        get_node_answers(node->answers, node_obj);
        
		nodes.Add(node->id, node);
	}
}

void UDialogManager::get_dialog_answers(TMap<INT16, smartFDialogAnswer>& answers, TSharedPtr<FJsonObject>& jobj)
{
    TArray<TSharedPtr<FJsonValue>> answers_json = jobj->GetArrayField(TEXT("answers"));
	for (int32 i=0; i<answers_json.Num(); ++i)
	{       
		TSharedPtr<FJsonValue> value = answers_json[i];
		TSharedPtr<FJsonObject> ans_obj = value->AsObject();
        
        smartFDialogAnswer answer(new FDialogAnswer());

		answer->id = ans_obj->GetIntegerField(TEXT("id"));
		answer->text = ans_obj->GetStringField(TEXT("text"));
		answer->malk_text = ans_obj->GetStringField(TEXT("malk_text"));
		answer->effect = ans_obj->GetStringField(TEXT("effect"));
		answer->condition = ans_obj->GetStringField(TEXT("condition"));
		answer->next_node = ans_obj->GetIntegerField(TEXT("react"));
        
		answers.Add(answer->id, answer);
	}
}

void UDialogManager::get_dialog_starts(TArray<FDialogStart>& starts, TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> starts_json = jobj->GetArrayField(TEXT("starts"));
	for (int32 i = 0; i < starts_json.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = starts_json[i];
		TSharedPtr<FJsonObject> start_obj = value->AsObject();

		FDialogStart start;

		start.id = start_obj->GetIntegerField(TEXT("id"));
		start.condition = start_obj->GetStringField(TEXT("condition"));

		starts.Add(start);
	}
}

void UDialogManager::get_node_answers(TArray<INT16>& answers, TSharedPtr<FJsonObject>& jobj)
{
    TArray<TSharedPtr<FJsonValue>> answers_json = jobj->GetArrayField(TEXT("answers"));
	for (int32 i=0; i< answers_json.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = answers_json[i];
        INT16 answer = value->AsNumber();
        
        answers.Add(answer);
	}
	//UE_LOG(LogTemp, Error, TEXT("parsed ansvers %d"), answers.Num());
}