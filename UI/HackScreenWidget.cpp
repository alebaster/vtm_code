#include "HackScreenWidget.h"

#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"
#include "Components/HackComponent.h"

bool UHackScreenWidget::Initialize()
{
	Super::Initialize();

	return true;
}

void UHackScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
	//if (Header)
	//	Header->SetText(FText::FromString("sss"));

	if (Input)
	{
		Input->OnTextCommitted.AddDynamic(this, &UHackScreenWidget::OnTextCommitted);
		Input->OnTextChanged.AddDynamic(this, &UHackScreenWidget::OnTextChanged);
	}
}

FEventReply UHackScreenWidget::OnKeyPress(FKeyEvent InKeyEvent)
{
	FKey k = InKeyEvent.GetKey();

	if (k == "Escape")
	{
		AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (controller)
			controller->EndUse();
		//AMainCharacter* player_character = Cast<AMainCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//if (player_character)
		//	player_character->WidgetEscape();

	}

	return FEventReply(false);
}

void UHackScreenWidget::OnTextCommitted(const FText& text, ETextCommit::Type type)
{
	if (type == ETextCommit::OnEnter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTextCommitted OnEnter"));

		ClearInput();

		if (hack_component)
			hack_component->Command(text);
	}
}

void UHackScreenWidget::OnTextChanged(const FText& text)
{
	FString k = text.ToString();

	if (k == "N" || k == "P" || k == "D" || k == "M" || k == "Q")
	{
		if (hack_component->Command(FText::FromString(k)))
			ClearInput();
	}
}

void UHackScreenWidget::ClearInput()
{
	if (Input)
		Input->SetText(FText::FromString(""));
}

void UHackScreenWidget::SetHeader(TArray<FString>& lines)
{
	if (!Header)
		return;

	FString s = "+----------------------------------------+\n"; // 42
			//s+= "|                                        |\n";

			for (int i=0;i<lines.Num();++i)
			{
				s += "|";
				int free_space = 42-lines[i].Len();
				for (int j=0;j<free_space/2;++j)
					s += " ";
				s += lines[i];
				for (int j=free_space/2+lines[i].Len()+1;j<41;++j)
					s += " ";
				s += "|\n";
			}

			//s+= "|                                        |\n";
			s+= "+----------------------------------------+";

	Header->SetText(FText::FromString(s));
}

void UHackScreenWidget::SetHeader(FString line)
{
	if (!Header)
		return;

	if (line == "")
	{
		Header->SetText(FText::FromString(""));
		return;
	}

	FString s = "+----------------------------------------+\n"; // 42

	s += "|";
	int free_space = 42 - line.Len();
	for (int j = 0; j<free_space / 2; ++j)
		s += " ";
	s += line;
	for (int j = free_space / 2 + line.Len() + 1; j<41; ++j)
		s += " ";
	s += "|\n";
	s += "+----------------------------------------+";

	Header->SetText(FText::FromString(s));
}

void UHackScreenWidget::SetBody(FString s)
{
	if (!Body)
		return;

	Body->SetText(FText::FromString(s));
}

void UHackScreenWidget::SetHelper(FString s)
{
	if (!Helper)
		return;

	Helper->SetText(FText::FromString(s));
}

void UHackScreenWidget::SetBottom(FString s)
{
	if (!Input || !Invite)
		return;

	Invite->SetText(FText::FromString(s));

	Input->SetKeyboardFocus();
}
