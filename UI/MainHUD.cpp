#include "MainHUD.h"

#include "Styling/SlateBrush.h"
#include "Styling/SlateColor.h"
#include "Math/Color.h"

#include "Character/MainCharacter.h"
#include "VtmPlayerController.h"
#include "Actors/DoorActor.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();

	Crosshair_Active = Crosshair_tex_Default;
}

bool UMainHUD::Initialize()
{
	Super::Initialize();

	if (!Crosshair)
		UE_LOG(LogTemp, Error, TEXT("Crosshair not binded"));

	if (!UseTex)
		UE_LOG(LogTemp, Error, TEXT("UseTex not binded"));

	if (!UseBack)
		UE_LOG(LogTemp, Error, TEXT("UseBack not binded"));

	if (!LockpickingOverlay)
		UE_LOG(LogTemp, Error, TEXT("LockpickingOverlay not binded"))
	else
		LockpickingOverlay->SetVisibility(ESlateVisibility::Hidden);

	if (!LockpickingBar)
		UE_LOG(LogTemp, Error, TEXT("LockpickingBar not binded"))
	else
		LockpickingBar->SetVisibility(ESlateVisibility::Hidden);

	if (!InfoText)
		UE_LOG(LogTemp, Error, TEXT("InfoText not binded"))
	else
		InfoText->SetVisibility(ESlateVisibility::Hidden);

	if (!InfoImageLeft)
		UE_LOG(LogTemp, Error, TEXT("InfoImageLeft not binded"))
	else
		InfoImageLeft->SetVisibility(ESlateVisibility::Hidden);

	if (!InfoImageRight)
		UE_LOG(LogTemp, Error, TEXT("InfoImageRight not binded"))
	else
		InfoImageRight->SetVisibility(ESlateVisibility::Hidden);

	WeaponImage->SetVisibility(ESlateVisibility::Hidden);

	return true;
}

void UMainHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Lockpicking();

	if (!notifications_queue.IsEmpty() && !GetWorld()->GetTimerManager().IsTimerActive(TimerHandle))
	{
		TPair<uint8, FString> pair;
		notifications_queue.Dequeue(pair);
		ShowNotification(pair.Key,pair.Value);
	}
}

void UMainHUD::Show()
{
	AddToViewport(0);

}

void UMainHUD::Hide()
{
	//RemoveFromViewport();
	RemoveFromParent();
}

void UMainHUD::ShowCrosshair()
{
	if (Crosshair)
		Crosshair->SetVisibility(ESlateVisibility::Visible);
}

void UMainHUD::HideCrosshair()
{
	if (Crosshair)
		Crosshair->SetVisibility(ESlateVisibility::Hidden);
}

void UMainHUD::ShowUseTex()
{
	if (UseTex && UseBack)
	{
		UseBack->SetVisibility(ESlateVisibility::Visible);
		UseTex->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHUD::HideUseTex()
{
	if (UseTex && UseBack)
	{
		//UseBack->SetColorAndOpacity(FLinearColor(1,0,0,0));
		//UseTex->SetColorAndOpacity(FLinearColor(1,0,0,0));
		UseBack->SetVisibility(ESlateVisibility::Hidden);
		UseTex->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainHUD::SetUseTex(UTexture2D* tex)
{
	if (UseTex)
		UseTex->SetBrushFromTexture(tex);
}

void UMainHUD::ShowExp(int amount)
{
	FString s = "Experience Rewarded: " + FString::FormatAsNumber(amount);

	notifications_queue.Enqueue(TPair<uint8,FString>(0,s));
	//ShowNotification(0,s);
}

void UMainHUD::ShowQuestUpd()
{
	FString s = "Quest Log Updated";

	notifications_queue.Enqueue(TPair<uint8, FString>(1, s));
	//ShowNotification(1, s);
}

void UMainHUD::ShowQuestEnd(FString quest_id)
{
	FString s = "";

	notifications_queue.Enqueue(TPair<uint8, FString>(2, s));
	//ShowNotification(2, s);
}

void UMainHUD::ShowMoney(int amount)
{
	FString s = "";

	notifications_queue.Enqueue(TPair<uint8, FString>(3, s));
	//ShowNotification(3, s);
}

void UMainHUD::ShowLockSucc(int difficulty)
{
	FString s = "Lockpicking succeeded at lock difficulty of "+FString::FromInt(difficulty);

	ShowNotification(4, s);
}

void UMainHUD::ShowItemTake(FString name)
{
	FString s = "Item Gained: "+name;

	ShowNotification(5, s);
}

void UMainHUD::ShowLockNeed()
{
	ShowNotification(5, "Need Lockpick");
}

void UMainHUD::ShowNotification(uint8 type, FString text)
{
	if (!InfoText || !InfoImageLeft || !InfoImageRight)
		return;

	USoundWave* sound = 0;
	UTexture2D* tex = 0;

	switch (type)
	{
	case 0:
		sound = NotificationSound_Exp;
		tex = NotificationImage_Exp;
		break;
	case 1:
		sound = NotificationSound_QuestUpd;
		tex = NotificationImage_QuestUpd;
		break;
	case 2:
		sound = NotificationSound_QuestEnd;
		tex = NotificationImage_QuestEnd;
		break;
	case 3:
		sound = NotificationSound_Money;
		tex = NotificationImage_Money;
		break;
	case 4:
		sound = NotificationSound_LockSucc;
		tex = NotificationImage_LockSucc;
		break;
	case 5:
		sound = NotificationSound_ItemTake;
		tex = NotificationImage_ItemTake;
		break;
	case 6:
		sound = 0;
		tex = NotificationImage_LockNeed;
		break;
	default:
		break;
	}

	if (sound)
		UGameplayStatics::PlaySound2D(GetWorld(), sound);

	if (tex)
	{
		InfoText->SetVisibility(ESlateVisibility::Visible);
		InfoImageLeft->SetVisibility(ESlateVisibility::Visible);
		InfoImageRight->SetVisibility(ESlateVisibility::Visible);

		InfoText->SetText(FText::FromString(text));
		InfoImageLeft->SetBrushFromTexture(tex);
		InfoImageRight->SetBrushFromTexture(tex);

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMainHUD::AutoHideNotification, 2.0f, true);
	}
}

void UMainHUD::AutoHideNotification()
{
	InfoText->SetVisibility(ESlateVisibility::Hidden);
	InfoImageLeft->SetVisibility(ESlateVisibility::Hidden);
	InfoImageRight->SetVisibility(ESlateVisibility::Hidden);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UMainHUD::AddToNotificationQueue(uint8 type, FString string)
{
	notifications_queue.Enqueue(TPair<uint8, FString>(type, string));
}

void UMainHUD::SetCrosshairDefault()
{
	Crosshair_Active = Crosshair_tex_Default;
}

void UMainHUD::SetCrosshairGreen()
{
	Crosshair_Active = Crosshair_tex_Green;
}

void UMainHUD::StartLockpicking(ADoorActor* door)
{
	if (!LockpickingOverlay || !LockpickingBar)
		return;

	LockpickingOverlay->SetVisibility(ESlateVisibility::Visible);
	LockpickingBar->SetVisibility(ESlateVisibility::Visible);

	//s = v * 3;
	speed = 1.0f/lockpicking_time;
	start_time = GetWorld()->GetRealTimeSeconds();
	b_lockpicking = true;

	_door = door;
	if (door)
		door->PlayLockpicking();
}

void UMainHUD::Lockpicking()
{
	if (!b_lockpicking)
		return;

	float distance = (float)(speed * (GetWorld()->GetRealTimeSeconds() - start_time));

	LockpickingBar->SetPercent(distance);

	if (distance > 0.99f)
	{
		EndLockpicking();
		b_lockpicking = false;
	}
}

void UMainHUD::EndLockpicking()
{
	if (!LockpickingOverlay || !LockpickingBar)
		return;

	LockpickingOverlay->SetVisibility(ESlateVisibility::Hidden);
	LockpickingBar->SetVisibility(ESlateVisibility::Hidden);

	//AMainCharacter* owner = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	//if (owner)
	//	owner->EndLockpicking();

	AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
		controller->EndUse();

	if (_door)
	{
		// check difficulty level
		_door->PlayLockpickingSuccess();
		ShowLockSucc(_door->Difficulty);
		_door->Unlock();
	}
}

void UMainHUD::ShowWeaponImage(UTexture2D* tex)
{
	if (WeaponImage)
	{
		WeaponImage->SetBrushFromTexture(tex);
		WeaponImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHUD::HideWeaponImage()
{
	if (WeaponImage)
	{
		WeaponImage->SetBrushFromTexture(0);
		WeaponImage->SetVisibility(ESlateVisibility::Hidden);
	}
}