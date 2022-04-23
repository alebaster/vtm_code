#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include <string>
#include <luacppinterface.h>

#include "LuaComponent.generated.h"

#define LUA_EVAL(func,arg) func##"(\"" + arg + "\")"
#define LUA_EVAL_S(func,arg) func##"(\""##arg##"\")"

//class Lua;
class UDialogManager;
class UQuestManager;
class UInventoryComponent;
class UWodComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API ULuaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULuaComponent();

	FInventoryItemBase* test();

	int ExecuteString(FString& script);
	int ExecuteStringWBoolResult(FString& script);
	void ExecuteStringWOResult(FString& script);

	int GetQuestImpl(std::string quest_id);
	void SetQuestImpl(std::string quest_id, int stage_id);
	int GetQuestValueImpl(std::string quest_id, std::string var);
	void SetQuestValueImpl(std::string quest_id, std::string var, int value);
	int GetGlobalValueImpl(std::string var);
	void SetGlobalValueImpl(std::string var, int value);
	bool HasItemImpl(std::string id);
	void GiveExpImpl(int amount);
	void GiveMoneyImpl(int amount);

protected:
	virtual void BeginPlay() override;

private:
	Lua lua;
	//Lua* lua;
	//TSharedPtr<Lua> lua;

	UPROPERTY()
	UDialogManager* dialog_manager=0;

	UPROPERTY()
	UQuestManager* quests_manager=0;

	UPROPERTY()
	UInventoryComponent* inventory_manager=0;

	UPROPERTY()
	UWodComponent* wod_manager = 0;
};
