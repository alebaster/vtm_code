#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/Overlay.h"
#include "Runtime/UMG/Public/Components/ProgressBar.h"

#include "Containers/Queue.h"
#include "Containers/Map.h"

#include "MainHUD.generated.h"

class ADoorActor;

UCLASS()
class VTM_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void Show();
	void Hide();

	void ShowCrosshair();
	void HideCrosshair();

	void ShowUseTex();
	void HideUseTex();
	void SetUseTex(UTexture2D* tex);

	void ShowExp(int amount);
	void ShowQuestUpd();
	void ShowQuestEnd(FString quest_id);
	void ShowMoney(int amount);
	void ShowItemTake(FString name);
	void ShowLockSucc(int difficulty);
	void ShowLockNeed();

	void SetCrosshairDefault();
	void SetCrosshairGreen();

	void StartLockpicking(ADoorActor* door);
	void Lockpicking();
	void EndLockpicking();

	void ShowWeaponImage(UTexture2D* tex);
	void HideWeaponImage();

	UPROPERTY(EditAnywhere)
	float lockpicking_time = 3.0f;
	
protected:
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//FEventReply OnKeyDown(FKeyEvent InKeyEvent);

private:
	// 0 - exp, 1 - quest upd, 2 - quest end, 3 - money, 4 - lock, 5 - item take, 6 - need lock
	void ShowNotification(uint8 type, FString text);

	UFUNCTION()
	void AutoHideNotification();

	TQueue<TPair<uint8, FString>> notifications_queue;
	void AddToNotificationQueue(uint8 type, FString string);

	UPROPERTY(meta = (BindWidget))
	UImage* Crosshair=0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* Crosshair_tex_Default;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* Crosshair_tex_Green;

	//UPROPERTY(EditAnywhere, Category = Variables, meta = (BindWidget))
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTexture2D* Crosshair_Active;

	// icon
	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_Exp = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_QuestEnd = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_QuestUpd = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_Money = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_LockSucc = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_ItemTake = 0;

	UPROPERTY(EditAnywhere, Category = Default)
	UTexture2D* NotificationImage_LockNeed = 0;

	// sound
	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_Exp;

	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_QuestEnd;

	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_QuestUpd;

	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_Money;

	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_LockSucc;

	UPROPERTY(EditAnywhere, Category = Default)
	USoundWave* NotificationSound_ItemTake;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UImage* UseBack=0;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UImage* UseTex=0;

	// notification
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText = 0;

	UPROPERTY(meta = (BindWidget))
	UImage* InfoImageLeft = 0;

	UPROPERTY(meta = (BindWidget))
	UImage* InfoImageRight = 0;

	// weapon
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage = 0;

	bool b_lockpicking = false;
	double start_time = 0.0f;
	float speed = 1.0f;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UOverlay* LockpickingOverlay=0;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UProgressBar* LockpickingBar=0;

	ADoorActor* _door;

	UPROPERTY()
	FTimerHandle TimerHandle;
};
