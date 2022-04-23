#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "Actors/UsableActor.h"

#include "HackComponent.generated.h"

class UHackScreenWidget;
class AMainCharacter;
class ULuaComponent;

USTRUCT()
struct FHackEmailStruct
{
	GENERATED_BODY()

	bool readed;
	bool deleted;
	FString subject;
	FString sender;
	FString body;
	FString dependency;
	FString runscript;
};

USTRUCT()
struct FHackStruct
{
	GENERATED_BODY()

	FString screen_saver;
	FString email_username;
	FString email_password;
	TArray<FString> LogonScreen;
	TArray<FString> menus;
	TArray<FString> commands;

	TArray<FHackEmailStruct> emails;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UHackComponent : public UActorComponent, public IUsableActor
{
	GENERATED_BODY()

public:	
	UHackComponent();

	UPROPERTY(EditAnywhere)
	FString id;

	UFUNCTION()
	virtual UTexture2D* GetActionImage() override;

	UFUNCTION()
	virtual int Use(AActor* who) override;

	UFUNCTION()
	virtual void EndUse(AActor* who) override;

	bool Command(FText s);

	UPROPERTY(EditAnywhere)
	USoundWave* UseSound;

	UPROPERTY(EditAnywhere)
	USoundWave* AcceptSound;

	UPROPERTY(EditAnywhere)
	USoundWave* ErrorSound;

	UPROPERTY(EditAnywhere)
	USoundWave* TypingSound;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UHackScreenWidget* hack_screen=0;

protected:
	virtual void BeginPlay() override;

private:
	//
	UPROPERTY()
	AMainCharacter* character;

	UPROPERTY()
	FString character_name;

	UPROPERTY()
	ULuaComponent* lua=0;

	// data
	UPROPERTY()
	FHackStruct HackData;

	UPROPERTY()
	FString current_menu;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UTexture2D* ActionImage;
	
	UPROPERTY()
	UHackScreenWidget* hack_screen = 0;

	void refresh_email(int& email_count, int& unread_count);
	FString create_body();
	bool check_login();
	bool check_password(FString s);
	int current_email_count=0;
	int current_letter=0;
	TArray<int> current_email_list;

	int get_letter_id(int num);
	TMap<int, int> letter_num_to_id;

	bool blogin = false;

	// json processing
	void ParseJson(FString& string);
	void FillStructs(TSharedPtr<FJsonObject> jobj);
	void get_logon(TArray<FString>& lines, TSharedPtr<FJsonObject>& hack);
	void get_menus(TArray<FString>& menus, TSharedPtr<FJsonObject>& hack);
	void get_commands(TArray<FString>& commands, TSharedPtr<FJsonObject>& hack);
	void get_emails(TArray<FHackEmailStruct>& emails, TSharedPtr<FJsonObject>& hack);
};
