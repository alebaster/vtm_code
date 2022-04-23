#include "HackComponent.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"

#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"
#include "UI/HackScreenWidget.h"

UHackComponent::UHackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHackComponent::BeginPlay()
{
	Super::BeginPlay();

	FString path = FPaths::ProjectContentDir() + "RuntimeResources/hack/" + id + ".json";
	FString json_string;
	if (FFileHelper::LoadFileToString(json_string, *path))
		ParseJson(json_string);
	else
		UE_LOG(LogTemp, Error, TEXT("cannot open hack file: %s with id: %s"), *path, *id);

	AActor* owner = GetOwner();
	if (owner)
	{
		UWidgetComponent* w = Cast<UWidgetComponent>(owner->GetComponentByClass(UWidgetComponent::StaticClass()));
		if (w)
		{
			w->InitWidget(); // why???
			hack_screen = Cast<UHackScreenWidget>(w->GetUserWidgetObject());
		}
	}

	if (!hack_screen)
		UE_LOG(LogTemp, Error, TEXT("cannot get UHackScreenWidget"))
	else
		hack_screen->SetHackComponent(this);
}

UTexture2D* UHackComponent::GetActionImage()
{
	return ActionImage;
}

int UHackComponent::Use(AActor* who)
{
	if (!hack_screen)
		return 0;

	UGameplayStatics::PlaySound2D(GetWorld(), UseSound);

	// move character camera to screen
	character = Cast<AMainCharacter>(who);
	if (character)
	{
		lua = Cast<ULuaComponent>(character->GetComponentByClass(ULuaComponent::StaticClass()));
		if (!lua)
			UE_LOG(LogTemp, Error, TEXT("cannot get lua_component from owner"))

		//UWidgetComponent* w = Cast<UWidgetComponent>(GetOwner()->GetComponentByClass(UWidgetComponent::StaticClass()));;
		//character->MoveCameraTo(w->GetComponentTransform());
		UStaticMeshComponent* m = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (m)
			character->MoveCameraTo(m->GetSocketTransform("LookSocket"));
	}

	current_menu = "home";

	// fill screen widget
	hack_screen->SetHeader(HackData.LogonScreen);
	hack_screen->SetBody(create_body());
	character_name = "noname";
	if (character)
		character_name = character->GetPlayerName();
	hack_screen->SetBottom("["+character_name+"@home]");
	//hack_screen->SetKeyboardFocus();
    
    return 4;
}

void UHackComponent::EndUse(AActor* who)
{
	if (!hack_screen)
		return;

	//hack_screen->SetKeyboardFocus();
}

bool UHackComponent::Command(FText s)
{
	bool ret = true;

	FString ss = s.ToString();

	if (ss == "")
	{
		current_menu = "home";
		hack_screen->SetHeader(HackData.LogonScreen);
		hack_screen->SetBody(create_body());
		hack_screen->SetHelper("Type menu or command");
		hack_screen->SetBottom("[" + character_name + "@home]");
	}
	else if (ss == "help")
	{
		current_menu = "help";
		hack_screen->SetHeader("Help Information");
		hack_screen->SetBody(create_body());
		hack_screen->SetHelper("Type menu or command");
		hack_screen->SetBottom("[" + character_name + "@home]");
	}
	else if (ss == "email")
	{
		if (!check_login())
		{
			current_menu = "password";

			hack_screen->SetHeader("Password required");
			hack_screen->SetBody("");
			hack_screen->SetHelper("");
			hack_screen->SetBottom("Password: ");
		}
		else
		{
			current_menu = "email";

			hack_screen->SetHeader("Email for "+HackData.email_username);
			hack_screen->SetBody(create_body());
			hack_screen->SetHelper(FString::FromInt(current_email_count)+" total emails, choose 1..."+FString::FromInt(current_email_count)+"\n"+
								   "[n]ext or [p]rev for more emails");
			hack_screen->SetBottom("[q]uit to exit: ");
		}
	}
	else if (s.IsNumeric())
	{
		//UE_LOG(LogTemp, Warning, TEXT("entered num %d"), FCString::Atoi(*ss));
		if (letter_num_to_id.Contains(FCString::Atoi(*ss)))
		{
			current_menu = "letter";
			current_letter = FCString::Atoi(*ss);

			FHackEmailStruct* letter = &HackData.emails[get_letter_id(current_letter)];
			letter->readed = true;

			hack_screen->SetHeader("");
			hack_screen->SetBody(create_body());
			hack_screen->SetHelper("[n]ext, [p]rev, [d]elete, [m]enu");
			hack_screen->SetBottom("[q]uit: ");

			lua->ExecuteStringWOResult(letter->runscript);

			//hack_screen->ClearInput();
		}
	}
	else if (ss == "n")
	{
		if (current_menu == "letter")
		{
			int maybe_next_index = 0;
			if (letter_num_to_id.Contains(current_letter+1))
			{
				//hack_screen->ClearInput();
				Command(FText::FromString(FString::FromInt(current_letter+1)));
			}
		}
		else
			ret = false;
	}
	else if (ss == "p")
	{
		if (current_menu == "letter")
		{
			int maybe_prev_index = 0;
			if (letter_num_to_id.Contains(current_letter-1))
			{
				Command(FText::FromString(FString::FromInt(current_letter-1)));
				//hack_screen->ClearInput();
			}
		}
		else
			ret = false;
	}
	else if (ss == "d")
	{
		if (current_menu == "letter")
		{
			FHackEmailStruct* letter = &HackData.emails[get_letter_id(current_letter)];
			letter->deleted = true;

			//hack_screen->ClearInput();

			Command(FText::FromString("email"));
			
		}
		else
			ret = false;
	}
	else if (ss == "m")
	{
		if (current_menu == "letter")
		{
			Command(FText::FromString("email"));
			//hack_screen->ClearInput();
		}
		else
			ret = false;
	}
	else if (ss == "q")
	{
		if (current_menu == "email" || current_menu == "letter")
		{
			Command(FText::FromString(""));
			//hack_screen->ClearInput();
		}
		else
			ret = false;
	}
	else if (ss == "quit")
	{
		AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (controller)
			controller->EndUse();
		else
			ret = false;
	}
	else
	{
		if (current_menu == "password")
		{
			if (!check_password(ss))
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);

				hack_screen->SetHeader("PASSWORD FAILED");
				hack_screen->SetBody("[Press \'ENTER\' to go back]");
				hack_screen->SetHelper("");
				hack_screen->SetBottom("Password: ");
			}
			else
			{
				UGameplayStatics::PlaySound2D(GetWorld(), AcceptSound);

				current_menu = "password_success";

				hack_screen->SetHeader("Password succeeded");
				hack_screen->SetBody("Password accepted: <"+HackData.email_password+">\nEntering email.");
				hack_screen->SetHelper("");
				hack_screen->SetBottom("[Press \'ENTER\' to continue]");
			}
		}
		else if (current_menu == "password_success")
		{
			Command(FText::FromString("email"));
		}
		else
		{
			UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);

			current_menu = "error";
			hack_screen->SetHeader("Invalid command");
			hack_screen->SetBody(create_body());
			hack_screen->SetHelper("");
			hack_screen->SetBottom("[Press \'ENTER\' to continue]");
		}
	}

	return ret;
}

FString UHackComponent::create_body()
{
	FString s;

	if (current_menu == "home")
	{
		int email_count = 0;
		int unread_count = 0;

		refresh_email(email_count, unread_count);

		s += "You have "+ FString::FromInt(email_count) + " emails, ";
		s += FString::FromInt(unread_count) + " are unread";

		s += "\n";
		s += "\n";

		s += "Home menu:\n";
		s += "\n";

		s += "Avaiable menus:\n";
		for (auto menu : HackData.menus)
			s += "\t" + menu + "\n";

		s += "\n";

		s += "Avaiable commands:\n";
		for (auto command : HackData.commands)
			s += "\t" + command + "\n";
	}
	else if (current_menu == "email")
	{
		letter_num_to_id.Empty();

		int email_count = 0;
		int unread_count = 0;
		refresh_email(email_count, unread_count);

		int num = 1;
		for (auto i : current_email_list)
		{
			FHackEmailStruct* mail = &HackData.emails[i];
			s += "[" + FString::FromInt(num);
			if (!mail->readed)
				s += "*]";
			else
				s += "]";
			s += mail->subject + "\n";

			letter_num_to_id.Add(num,i);

			num++;
		}
	}
	else if (current_menu == "letter")
	{
		FHackEmailStruct* letter = &HackData.emails[get_letter_id(current_letter)];

		s += "(Subject) " + letter->subject;
		s += "\n";
		s += "(From) " + letter->sender;
		s += "\n";
		s += "\n";
		s += letter->body;
	}
	else if (current_menu == "help")
	{
		s += "Type \'list\' to get all avaiable menus and commands\n";
	}
	else if (current_menu == "error")
	{
		s += "Type \'list\' to get all the avaiable commands\n";
		s += "Type \'help\' for assistance\n";
	}

	return s;
}

void UHackComponent::refresh_email(int& email_count, int& unread_count)
{
	current_email_list.Empty();

	for (int i = 0; i<HackData.emails.Num(); ++i)
	{
		FHackEmailStruct& e = HackData.emails[i];

		if (e.deleted)
			continue;
		if (lua)
		{
			if (!lua->ExecuteString(e.dependency))
				continue;
		}
		if (!e.readed)
			unread_count++;

		email_count++;

		current_email_list.Add(i);
	}

	current_email_count = email_count;
}

bool UHackComponent::check_login()
{
	return blogin;
}

bool UHackComponent::check_password(FString s)
{
	blogin = s==HackData.email_password;

	return blogin;
}

int UHackComponent::get_letter_id(int num)
{
	int* ret = letter_num_to_id.Find(num);

	if (ret)
		return *ret;
	else
		return 0;
}

// json processing
void UHackComponent::ParseJson(FString& string)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(string);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("json hack file with id: %s not valid"), *id);
		return;
	}

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FillStructs(JsonObject);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("cannot decerialize json hack file with id: %s"), *id);
}

void UHackComponent::FillStructs(TSharedPtr<FJsonObject> jobj)
{
	HackData.screen_saver = jobj->GetStringField(TEXT("screen_saver"));
	HackData.email_username = jobj->GetStringField(TEXT("email_username"));
	HackData.email_password = jobj->GetStringField(TEXT("email_password"));
	get_logon(HackData.LogonScreen, jobj);
	get_menus(HackData.menus, jobj);
	get_commands(HackData.commands, jobj);
	get_emails(HackData.emails, jobj);
}

void UHackComponent::get_logon(TArray<FString>& lines, TSharedPtr<FJsonObject>& hack)
{
	TArray<TSharedPtr<FJsonValue>> json_lines = hack->GetArrayField(TEXT("LogonScreen"));
	for (int32 i = 0; i<json_lines.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_lines[i];
		FString line = value->AsString();

		lines.Add(line);
	}
}

void UHackComponent::get_menus(TArray<FString>& menus, TSharedPtr<FJsonObject>& hack)
{
	TArray<TSharedPtr<FJsonValue>> json_menus = hack->GetArrayField(TEXT("menus"));
	for (int32 i = 0; i<json_menus.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_menus[i];
		FString menu = value->AsString();

		menus.Add(menu);
	}
}

void UHackComponent::get_commands(TArray<FString>& commands, TSharedPtr<FJsonObject>& hack)
{
	TArray<TSharedPtr<FJsonValue>> json_commands = hack->GetArrayField(TEXT("commands"));
	for (int32 i = 0; i<json_commands.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_commands[i];
		FString command = value->AsString();

		commands.Add(command);
	}
}

void UHackComponent::get_emails(TArray<FHackEmailStruct>& emails, TSharedPtr<FJsonObject>& hack)
{
	TArray<TSharedPtr<FJsonValue>> json_emails = hack->GetArrayField(TEXT("Emails"));
	for (int32 i = 0; i<json_emails.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_emails[i];
		TSharedPtr<FJsonObject> email = value->AsObject();
	
		FString subject = email->GetStringField(TEXT("subject"));
		FString sender = email->GetStringField(TEXT("sender"));
		FString body = email->GetStringField(TEXT("body"));
		FString dependency;// = email->GetStringField(TEXT("dependency"));
		FString runscript; // = email->GetStringField(TEXT("runscript"));
		email->TryGetStringField(TEXT("dependency"), dependency);
		email->TryGetStringField(TEXT("runscript"), runscript);
	
		FHackEmailStruct email_struct{ 0,0,subject,sender,body,dependency,runscript };
		emails.Add(email_struct);
	}
}