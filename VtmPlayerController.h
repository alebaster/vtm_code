#pragma once

// ue
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

// ui
#include "UI/MainMultiWidget.h"
#include "UI/InventoryWidget.h"

// vtm
#include "Inventory/InventoryComponent.h"

#include "VtmPlayerController.generated.h"

UCLASS()
class VTM_API AVtmPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AVtmPlayerController();
    
	bool character_in_use = false;

	void EnableControls();
	void EndUse();
	void EscapePressed();
	void StartBarter(TArray<FLiteInvItem>& inv);

	void ShowMouseCursor(bool show);
	void BlockMouseControl(bool block);
	void BlockKeyboardControl(bool block);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad) override;

private:
	int last_use_type=0;

    void Use();
	void CharacterPressed();
	void InventoryPressed();
	void JournalPressed();

	void TestPressed();
	void TestPressed2();

    //void ToggleKeyboardControl(bool enable);

	// components
	UPROPERTY() // inventory
	UInventoryComponent* inventory_component = 0;

	// UI
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainMultiWidget> mm_widget_bp = 0;
	UPROPERTY()
	UMainMultiWidget* mm_widget = 0;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventoryWidget> inv_widget_bp = 0;
	UPROPERTY()
	UInventoryWidget* inv_widget = 0;

};
