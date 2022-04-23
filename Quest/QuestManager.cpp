#include "QuestManager.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "Character/MainCharacter.h"
#include "UI/MainHUD.h"
#include "Lua/LuaComponent.h"

UQuestManager::UQuestManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UQuestManager::BeginPlay()
{
	Super::BeginPlay();

	FString path = FPaths::ProjectContentDir() + "RuntimeResources/quests/quests.json";

	FString json_string;
	if (FFileHelper::LoadFileToString(json_string, *path))
		ParseJson(json_string);
	else
		UE_LOG(LogTemp, Warning, TEXT("cannot open quests file %s"), *path);
}

const FQuestStageStruct* UQuestManager::GetQuestStage(const FString& quest_id, int stage_id) const
{
    const FQuestStruct* qsp = quests.Find(quest_id);
    if (qsp)
    {
        for (auto& s : qsp->stages)
        {
            if (s.id == stage_id)
                return &s;
        }
    }
    
    return 0;
}

FString UQuestManager::GetQuestName(const FString& quest_id)
{
    FString ret("");
    FQuestStruct* qsp = quests.Find(quest_id);
    if (qsp)
        ret = qsp->title;
    
    return ret;
}

INT16 UQuestManager::GetQuestArea(const FString& quest_id)
{
    INT16 ret = -1;
    FQuestStruct* qsp = quests.Find(quest_id);
    if (qsp)
        ret = qsp->area;
    
    return ret;
}

int UQuestManager::GetQuestCurrentStageId(FString& quest_id)
{
	int ret = 0;

	for (auto& p : active_quests_stages)
	{
		if (p.id == quest_id)
		{
			ret = p.stage_num;
			break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[UQuestManager] return stage id %d of quest %s"),ret,*quest_id)
	return ret;
}

void UQuestManager::SetQuestCurrentStageId(FString& quest_id, int stage_id)
{
	bool found=false;
	bool updated=false;
	for (auto& p : active_quests_stages)
	{
		if (p.id == quest_id)
		{
			found = true;
			if (p.stage_num > (INT16)stage_id)
				break;
			updated = p.stage_num != (INT16)stage_id;
			p.stage_num = (INT16)stage_id;
			break;
		}
	}

	if (!found)
	{
		FQuestStruct* qsp = quests.Find(quest_id);
		if (qsp)
		{
			active_quests_stages.Add(FActiveQuestStruct{ qsp->id, (INT16)stage_id });
			found = true;
			updated = true;
		}
	}

	if (found && updated)
	{
		AMainCharacter* owner = Cast<AMainCharacter>(GetOwner());
		if (owner)
		{
			UMainHUD* hud = owner->GetHud();
			if (hud)
				hud->ShowQuestUpd();
			else
				UE_LOG(LogTemp, Error, TEXT("hud in quest manager is null"));
		}
		else
			UE_LOG(LogTemp, Error, TEXT("owner in quest manager is null"));

		const FQuestStageStruct* qss = GetQuestStage(quest_id, stage_id);
		if (qss)
		{
			if (lua_component)
			{
				FString s = qss->effect;
				lua_component->ExecuteStringWOResult(s);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[UQuestManager] set quest %s to stage %d"), *quest_id, stage_id)
}

int UQuestManager::GetQuestVariable(FString& quest_id, FString& var)
{
	return 0;
}

void UQuestManager::SetQuestVariable(FString& quest_id, FString& var, int value)
{

}

int UQuestManager::GetGlobalVariable(FString& var)
{
	return 0;
}

void UQuestManager::SetGlobalVariable(FString& var, int value)
{

}

void UQuestManager::ParseJson(FString& string)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(string);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("json quests is not valid"));
		return;
	}

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
		FillStructs(JsonObject);
	else
		UE_LOG(LogTemp, Warning, TEXT("cannot deserialize json quests"));
}

void UQuestManager::FillStructs(TSharedPtr<FJsonObject> jobj)
{
	TArray<TSharedPtr<FJsonValue>> objArray = jobj->GetArrayField(TEXT("quests"));
	for (int32 i=0; i<objArray.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = objArray[i];
		TSharedPtr<FJsonObject> quest = value->AsObject();

		FString quest_id = quest->GetStringField(TEXT("id"));
		FString quest_title = quest->GetStringField(TEXT("title"));
		INT16 quest_area = (INT16)quest->GetIntegerField(TEXT("area"));
		TArray<FQuestStageStruct> quest_stages;
		get_stages(quest_stages, quest);

		FQuestStruct quest_struct{ quest_id,quest_title,quest_area,quest_stages };
		quests.Add(quest_id,quest_struct);
	}
}

void UQuestManager::get_stages(TArray<FQuestStageStruct>& stages, TSharedPtr<FJsonObject>& quest)
{
	TArray<TSharedPtr<FJsonValue>> json_stages = quest->GetArrayField(TEXT("stages"));
	for (int32 i = 0; i < json_stages.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_stages[i];
		TSharedPtr<FJsonObject> stage = value->AsObject();

		INT16 stage_id = (INT16)stage->GetIntegerField(TEXT("id"));
		INT16 stage_type = (INT16)stage->GetIntegerField(TEXT("type"));
		FString stage_effect;
		stage->TryGetStringField(TEXT("effect"), stage_effect);
		FString stage_descr = stage->GetStringField(TEXT("descr"));

		FQuestStageStruct quest_stage_struct{ stage_id,stage_type,stage_effect,stage_descr };
		stages.Add(quest_stage_struct);
	}
}