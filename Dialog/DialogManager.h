#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "DialogManager.generated.h"

struct FDialogNode;
struct FDialogAnswer;
struct FDialog;

class UDialogWidget;
class ULuaComponent;

#define smartFDialogNode TSharedPtr<FDialogNode>
#define smartFDialogAnswer TSharedPtr<FDialogAnswer> 
#define smartFDialog TSharedPtr<FDialog> 

USTRUCT()
struct FDialogNode
{
	GENERATED_BODY()

	INT16 id;
	FString text;
	FString effect;
	TArray<INT16> answers;
};

USTRUCT()
struct FDialogAnswer
{
	GENERATED_BODY()

	INT16 id;
	FString text;
	FString malk_text;
	FString effect;
	FString condition;
	INT16 next_node;
};

USTRUCT()
struct FDialogStart
{
	GENERATED_BODY()

	INT16 id;
	FString condition;
};

USTRUCT()
struct FDialog
{
	GENERATED_BODY()

	FString id;
	TArray<FDialogStart> starts;
	TMap<INT16, smartFDialogNode> nodes;
	TMap<INT16, smartFDialogAnswer> answers;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UDialogManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDialogManager();
    void SetLua(ULuaComponent* lua) {lua_component = lua;}
    
    void StartDialog(FString& id);
	void Skip();
	void ReactToAnswer(INT16 key);
	void EndDialog();
    
    FDialog* GetDialog(FString& id);
	//FDialogNodeStruct* GetDialogNode(INT16 id);
	//FDialogAnswerStruct* GetAnswer(INT16 id);

protected:
	virtual void BeginPlay() override;

private:
	TMap<FString,smartFDialog> dialogs;
	TMap<INT16, INT16> current_answers; // <visual num, id>

	void FillDialogWidget(FDialog* dialog, INT16 node_id);

    UPROPERTY()
	FString current_dialog_id="";

	UPROPERTY()
	INT16 current_node_id=0;
    
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UDialogWidget> dialog_widget_bp = 0;
    
	UPROPERTY()
	UDialogWidget* dialog_widget = 0;

	//UPROPERTY()
	//APlayerController* PlayerController = 0;

	UPROPERTY()
	ULuaComponent* lua_component = 0;

	// sound
	UAudioComponent* current_wav=0;

	FTimerHandle wav_TimerHandle;

	UFUNCTION()
	void WavEnded();
    
	void FindCurrentLevelDialogs();
	void ParseDialog(FString id);

	// json processing
	void ParseJson(FString& string, FString& id);
	void FillStructs(TSharedPtr<FJsonObject> jobj, FString& id);
    void get_dialog_nodes(TMap<INT16, smartFDialogNode>& nodes, TSharedPtr<FJsonObject>& jobj);
    void get_dialog_answers(TMap<INT16, smartFDialogAnswer>& answers, TSharedPtr<FJsonObject>& jobj);
	void get_dialog_starts(TArray<FDialogStart>& starts, TSharedPtr<FJsonObject>& jobj);
    void get_node_answers(TArray<INT16>& answers, TSharedPtr<FJsonObject>& jobj);
};
