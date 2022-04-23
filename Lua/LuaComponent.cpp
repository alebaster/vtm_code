#include "LuaComponent.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"

#include <algorithm>
#include <fstream>
#include <streambuf>
#include <string>
#include <iostream>
#include <luacppinterface.h>
#include <LuaTable.h>

#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"
#include "Dialog/DialogManager.h"
#include "Quest/QuestManager.h"
#include "Inventory/InventoryComponent.h"
#include "Character/WodComponent.h"

ULuaComponent::ULuaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULuaComponent::BeginPlay()
{
	Super::BeginPlay();

	FString path = FPaths::ProjectContentDir() + "RuntimeResources/reflection.lua";
	FString file_string;
	if (!FFileHelper::LoadFileToString(file_string, *path))
	{
		UE_LOG(LogTemp, Error, TEXT("cannot open item_db file"));
		return;
	}

	std::string script(TCHAR_TO_UTF8(*file_string));

	//std::ifstream file(TCHAR_TO_ANSI(*path));
	//std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	//lua = new Lua();
	lua.LoadStandardLibraries();
	std::string result = lua.RunScript(script);
	if (result != "No errors")
		UE_LOG(LogTemp, Error, TEXT("lua init FAILED"))
	else
		UE_LOG(LogTemp, Warning, TEXT("lua ready"));

	LuaTable global = lua.GetGlobalEnvironment();

	LuaFunction<int(std::string)> get_quest_func = lua.CreateFunction<int(std::string)>([this](std::string quest_id)
	{
		return GetQuestImpl(quest_id);
	});
	global.Set("GetQuestImpl", get_quest_func);

	LuaFunction<void(std::string, int)> set_quest_func = lua.CreateFunction<void(std::string, int)>([this](std::string quest_id, int stage_id)
	{
		return SetQuestImpl(quest_id, stage_id);
	});
	global.Set("SetQuestImpl", set_quest_func);

	LuaFunction<int(std::string, std::string)> get_variable_func = lua.CreateFunction<int(std::string, std::string)>([this](std::string quest_id, std::string var)
	{
		return GetQuestValueImpl(quest_id, var);
	});
	global.Set("GetQuestValueImpl", get_variable_func);

	LuaFunction<void(std::string, std::string, int)> set_variable_func = lua.CreateFunction<void(std::string, std::string, int)>([this](std::string quest_id, std::string var, int value)
	{
		return SetQuestValueImpl(quest_id, var, value);
	});
	global.Set("SetQuestValueImpl", set_variable_func);

	LuaFunction<int(std::string)> get_gloabl_variable_func = lua.CreateFunction<int(std::string)>([this](std::string var)
	{
		return GetGlobalValueImpl(var);
	});
	global.Set("GetGlobalValueImpl", get_variable_func);

	LuaFunction<void(std::string, int)> set_global_variable_func = lua.CreateFunction<void(std::string, int)>([this](std::string var, int value)
	{
		return SetGlobalValueImpl(var, value);
	});
	global.Set("SetGlobalValueImpl", set_variable_func);

	LuaFunction<bool(std::string)> has_item_func = lua.CreateFunction<bool(std::string)>([this](std::string var)
	{
		return HasItemImpl(var);
	});
	global.Set("HasItemImpl", has_item_func);

	LuaFunction<void(int)> give_exp_func = lua.CreateFunction<void(int)>([this](int var)
	{
		return GiveExpImpl(var);
	});
	global.Set("GiveExpImpl", give_exp_func);

	LuaFunction<void(int)> give_money_func = lua.CreateFunction<void(int)>([this](int var)
	{
		return GiveMoneyImpl(var);
	});
	global.Set("GiveMoneyImpl", give_money_func);

	// get components
	AMainCharacter* owner = Cast<AMainCharacter>(GetOwner());
	if (owner)
		dialog_manager = Cast<UDialogManager>(owner->GetComponentByClass(UDialogManager::StaticClass()));
	if (!dialog_manager)
		UE_LOG(LogTemp, Error, TEXT("dialog_manager in lua reflection system is null"));

	if (owner)
		quests_manager = Cast<UQuestManager>(owner->GetComponentByClass(UQuestManager::StaticClass()));
	if (!dialog_manager)
		UE_LOG(LogTemp, Error, TEXT("quests_manager in lua reflection system is null"));

	AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
		inventory_manager = Cast<UInventoryComponent>(controller->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (!inventory_manager)
		UE_LOG(LogTemp, Error, TEXT("inventory_manager in lua reflection system is null"));

	if (owner)
		wod_manager = Cast<UWodComponent>(owner->GetComponentByClass(UWodComponent::StaticClass()));
	if (!wod_manager)
		UE_LOG(LogTemp, Error, TEXT("wod_manager in lua reflection system is null"));

	//FString s = "Eval(\"GetQuest(\\\"ForkQuest\\\") == 0\")";
	////FString s = "Eval(\"1==0\")";
	//ExecuteString(s);

	// table test
	//LuaTable items = lua.GetGlobalEnvironment().Get<LuaTable>("sm_hideout_items");
	//UE_LOG(LogTemp, Error, TEXT("%d"), items.Get<int>("anaconda"));

	LuaTable loot = lua.GetGlobalEnvironment().Get<LuaTable>("sm_hideout_loot");
	LuaTable loot1 = loot.Get<LuaTable>("drawer_lootbox");
	//UE_LOG(LogTemp, Error, TEXT("%d"), loot1.GetTypeOfValueAt("id"));
	//std::string s = loot1.Get<std::string>("id");
	//UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(loot1.Get<std::string>("id").c_str()));
	//void ForAllStringKeys(std::function<void(std::string, LuaType::Value)> stringKeys) const;
}

FInventoryItemBase* ULuaComponent::test()
{
	FInventoryItemBase* ret=0;

	LuaTable inventory = lua.GetGlobalEnvironment().Get<LuaTable>("inventory");
	inventory.ForAllStringKeys([&](std::string item_id, LuaType::Value value)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s"), TCHAR_TO_UTF8(item_id.c_str()));
		FString i = item_id.c_str();
		ret = inventory_manager->AddItem(i);
	});

	return ret;
}

int ULuaComponent::ExecuteString(FString& script)
{
	//if (!lua)
	//	return false;

	if (script.IsEmpty())
		return true;

	FString fixed_script = script.ReplaceCharWithEscapedChar();
	std::string expr = "Eval(\""+ std::string(TCHAR_TO_UTF8(*fixed_script)) + "\")";

	UE_LOG(LogTemp, Warning, TEXT("lua eval string %s"), UTF8_TO_TCHAR(expr.c_str()));
	
	std::string result = lua.RunScript(expr);

	if (result != "No errors")
		UE_LOG(LogTemp, Error, TEXT("lua error evaluating %s: %s"), UTF8_TO_TCHAR(expr.c_str()), UTF8_TO_TCHAR(result.c_str()));

	UE_LOG(LogTemp, Warning, TEXT("lua result = %d"), lua.GetGlobalEnvironment().Get<int>("result"));

	return lua.GetGlobalEnvironment().Get<int>("result");
}

int ULuaComponent::ExecuteStringWBoolResult(FString& script)
{
	if (script.IsEmpty())
		return true;

	FString fixed_script = script.ReplaceCharWithEscapedChar();
	std::string expr = "Eval(\"" + std::string(TCHAR_TO_UTF8(*fixed_script)) + "\")";

	UE_LOG(LogTemp, Warning, TEXT("lua eval string %s"), UTF8_TO_TCHAR(expr.c_str()));

	std::string result = lua.RunScript(expr);

	if (result != "No errors")
		UE_LOG(LogTemp, Error, TEXT("lua error evaluating %s: %s"), UTF8_TO_TCHAR(expr.c_str()), UTF8_TO_TCHAR(result.c_str()));

	UE_LOG(LogTemp, Warning, TEXT("lua result = %d"), lua.GetGlobalEnvironment().Get<bool>("result"));

	return lua.GetGlobalEnvironment().Get<bool>("result");
}

void ULuaComponent::ExecuteStringWOResult(FString& script)
{
	//if (!lua)
	//	return;

	if (script.IsEmpty())
		return;

	FString fixed_script = script.ReplaceCharWithEscapedChar();
	std::string expr = "Do(\"" + std::string(TCHAR_TO_ANSI(*fixed_script)) + "\")";
	//std::replace(expr.begin(), expr.end(), '"','\"');

	std::string result = lua.RunScript(expr);
	if (result != "No errors")
		UE_LOG(LogTemp, Error, TEXT("lua error evaluating %s: %s"), UTF8_TO_TCHAR(expr.c_str()), UTF8_TO_TCHAR(result.c_str()));
}

int ULuaComponent::GetQuestImpl(std::string quest_id)
{
	if (!quests_manager)
		return -1;

	FString s = quest_id.c_str();
	
	return quests_manager->GetQuestCurrentStageId(s);
}

void ULuaComponent::SetQuestImpl(std::string quest_id, int stage_id)
{
	if (!quests_manager)
		return;

	FString s = quest_id.c_str();
	quests_manager->SetQuestCurrentStageId(s,stage_id);
}

int ULuaComponent::GetQuestValueImpl(std::string quest_id, std::string var)
{
	if (!quests_manager)
		return -1;

	FString q = quest_id.c_str();
	FString v = var.c_str();

	return quests_manager->GetQuestVariable(q, v);
}

void ULuaComponent::SetQuestValueImpl(std::string quest_id, std::string var, int value)
{
	if (!quests_manager)
		return;

	FString q = quest_id.c_str();
	FString v = var.c_str();

	quests_manager->SetQuestVariable(q, v, value);
}

int ULuaComponent::GetGlobalValueImpl(std::string var)
{
	if (!quests_manager)
		return -1;

	FString v = var.c_str();

	return quests_manager->GetGlobalVariable(v);
}

void ULuaComponent::SetGlobalValueImpl(std::string var, int value)
{
	if (!quests_manager)
		return;

	FString v = var.c_str();

	quests_manager->SetGlobalVariable(v, value);
}

bool ULuaComponent::HasItemImpl(std::string id)
{
	if (!inventory_manager)
		return false;

	FString q = id.c_str();
	UE_LOG(LogTemp, Warning, TEXT("check item: %d"), inventory_manager->HasItem(q));
	return inventory_manager->HasItem(q);
}

void ULuaComponent::GiveExpImpl(int amount)
{
	if (!wod_manager)
		return;

	wod_manager->GiveExp(amount);
}

void ULuaComponent::GiveMoneyImpl(int amount)
{
	if (!inventory_manager)
		return;

	inventory_manager->GiveMoney(amount);
}