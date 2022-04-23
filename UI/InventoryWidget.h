#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// umg
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"
#include "Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"
#include "Runtime/UMG/Public/Components/VerticalBox.h"

#include "Inventory/InventoryItem.h"


#include "InventoryWidget.generated.h"


class UInventoryItemWidget;
class UInventoryComponent;

UCLASS()
class VTM_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool OnScreen() { return IsInViewport(); }
    bool isBarter=false;

	void SetInventoryComponent(UInventoryComponent* inv) { inv_component = inv; }

	void ClickItem(UInventoryItemWidget* item_widget);

	void Show();
    void ShowBarter(TArray<FLiteInvItem>& inv);
	void Hide();
    void HideBarter();

	UPROPERTY(EditAnywhere)
	UTexture2D* GeneralImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* MeleeImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* RangedImage;

	UPROPERTY(EditAnywhere)
	UTexture2D* ArmorImage;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryItemWidget> inventory_item_widget_bp=0;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	// left
	UPROPERTY(meta = (BindWidget))
	UButton* LeftButton = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* RightButton = 0;

	UPROPERTY(meta = (BindWidget))
	UImage* CategoryImage = 0;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* InventoryBox = 0;

	// center
	UPROPERTY(meta = (BindWidget))
	UImage* InfoImage = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoTitle = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoDescription = 0;

	// bottom
	UPROPERTY(meta = (BindWidget))
	UButton* UseButton = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* DropButton = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton = 0;

	// loot area
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Loot = 0;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* LootBox = 0;
    
    // text for buttons
    UPROPERTY(meta = (BindWidget))
	UTextBlock* UseButtonText = 0;
    
    UPROPERTY(meta = (BindWidget))
	UTextBlock* DropButtonText = 0;

    // category
	UFUNCTION()
	void ClickLeftButton();

	UFUNCTION()
	void ClickRightButton();
    
    // item action
    UFUNCTION()
	void ClickUseButton();

	UFUNCTION()
	void ClickDropButton();

	UFUNCTION()
	void ClickExitButton();
    // ---

	void UpdatePage();
    void UpdateBarterPage();

	int8_t current_page = 0; // 0 - Items, 1 - MeleeWeapons, 2 - RangedWeapons, 3 - Armors

	UInventoryComponent* inv_component;

	FString base_icon_path;

    UInventoryItemWidget* current_widget=0;
	TArray<UInventoryItemWidget*> current_widgets;
    
    TArray<FLiteInvItem>* barter_inv=0;
};
