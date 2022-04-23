// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "QuestManager.generated.h"

USTRUCT()
struct FQuestStageStruct
{
	GENERATED_BODY()

	INT16 id;
	INT16 type;
	FString effect;
	FString descr;
};

USTRUCT()
struct FQuestStruct
{
	GENERATED_BODY()

	FString id;
	FString title;
	INT16 area;
	TArray<FQuestStageStruct> stages;
};

USTRUCT()
struct FActiveQuestStruct
{
	GENERATED_BODY()

	FString id;
	INT16 stage_num;
};

class ULuaComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UQuestManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestManager();
	void SetLua(ULuaComponent* lua) { lua_component = lua; }

	const TMap<FString, FQuestStruct>& GetQuests() const {return quests;}
	const TArray<FActiveQuestStruct>& GetActiveQuests() const {return active_quests_stages;}
    
    const FQuestStageStruct* GetQuestStage(const FString& quest_id, int stage_id) const;
    FString GetQuestName(const FString& quest_id);
    INT16 GetQuestArea(const FString& quest_id);

	int GetQuestCurrentStageId(FString& quest_id);
	void SetQuestCurrentStageId(FString& quest_id, int stage_id);
	int GetQuestVariable(FString& quest_id, FString& var);
	void SetQuestVariable(FString& quest_id, FString& var, int value);
	int GetGlobalVariable(FString& var);
	void SetGlobalVariable(FString& var, int value);

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	ULuaComponent* lua_component = 0;

	UPROPERTY()
	TMap<FString,FQuestStruct> quests;

	UPROPERTY()
	TArray<FActiveQuestStruct> active_quests_stages;

	// json processing
	void ParseJson(FString& string);
	void FillStructs(TSharedPtr<FJsonObject> jobj);
	void get_stages(TArray<FQuestStageStruct>& stages, TSharedPtr<FJsonObject>& quest);
};
