#include "InventoryWidget.h"

#include "Runtime/Core/Public/Misc/Paths.h"

#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"
#include "UI/InventoryItemWidget.h"
#include "Core/RuntimeUtils.h"
#include "Inventory/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UInventoryWidget::Initialize()
{
	Super::Initialize();

	base_icon_path = FPaths::ProjectContentDir() + "RuntimeResources/item_image/";

	if (LeftButton)
		LeftButton->OnClicked.AddDynamic(this, &UInventoryWidget::ClickLeftButton);
	else
		UE_LOG(LogTemp, Error, TEXT("LeftButton not binded"))

	if (RightButton)
		RightButton->OnClicked.AddDynamic(this, &UInventoryWidget::ClickRightButton);
	else
		UE_LOG(LogTemp, Error, TEXT("RightButton not binded"))

    if (UseButton)
		UseButton->OnClicked.AddDynamic(this, &UInventoryWidget::ClickUseButton);
	else
		UE_LOG(LogTemp, Error, TEXT("UseButton not binded"))
    
    if (DropButton)
		DropButton->OnClicked.AddDynamic(this, &UInventoryWidget::ClickDropButton);
	else
		UE_LOG(LogTemp, Error, TEXT("DropButton not binded"))
    
	if (ExitButton)
		ExitButton->OnClicked.AddDynamic(this, &UInventoryWidget::ClickExitButton);
	else
		UE_LOG(LogTemp, Error, TEXT("ExitButton not binded"))

	if (!inventory_item_widget_bp)
		UE_LOG(LogTemp, Error, TEXT("inventory_item_widget_bp is null"))

	return true;
}

void UInventoryWidget::Show()
{
	if (!OnScreen())
	{
		UpdatePage();
		AddToViewport(0);
	}
	else
		Hide();
}

void UInventoryWidget::Hide()
{
    HideBarter();
	RemoveFromViewport();
}

void UInventoryWidget::ShowBarter(TArray<FLiteInvItem>& inv)
{
    if (!Loot)
        return;
    
    barter_inv = &inv;
    
    UpdateBarterPage();
    
    // rename buttons???
    //if (UseButtonText)
    //    UseButtonText->SetText(TEXT("T"));
    
    Loot->SetVisibility(ESlateVisibility::Visible);
	LootBox->SetVisibility(ESlateVisibility::Visible);

	Show();
}

void UInventoryWidget::HideBarter()
{
    if (!Loot)
        return;
    
    barter_inv = 0;
    
    // child too?
    Loot->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::ClickItem(UInventoryItemWidget* item_widget)
{
	if (!item_widget)
		return;

	item_widget->SetActive();

	if (!inv_component)
		return;

	FInventoryItemBase* item = inv_component->GetItemFromDB(item_widget->id);

	if (!item)
	{
		UE_LOG(LogTemp, Error, TEXT("cant find item in ClickItem()"))
		return;
	}

	// update middle info widget
	if (InfoTitle)
		InfoTitle->SetText(FText::FromString(item->name));

	if (InfoDescription)
		InfoDescription->SetText(FText::FromString(item->description));

	if (InfoImage)
	{
		bool valid = false;
		int32 w, h;
		FString path = FPaths::ProjectContentDir() + "RuntimeResources/item_image_full/" + item->icon_name + ".png";
		UTexture2D* icon = URuntimeUtils::LoadTexture2D_FromFile(path, EImageFormat::PNG, valid, w, h);
		if (icon)
		{
			InfoImage->SetBrushFromTexture(icon);
			InfoImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
			InfoImage->SetVisibility(ESlateVisibility::Hidden);
	}

	for (auto w : current_widgets)
	{
		if (w != item_widget)
			w->SetPassive();
	}
    
    current_widget = item_widget;
}

void UInventoryWidget::UpdatePage()
{
	if (!InventoryBox)
		return;

	InventoryBox->ClearChildren();

	if (!inv_component)
		return;

	if (!inventory_item_widget_bp)
		return;

	current_widgets.Empty();

	EIIType type;

	UTexture2D* header_img=0;

	switch (current_page)
	{
	case 0:
		type = EIIType::etc;
		header_img = GeneralImage;
		break;
	case 1:
		type = EIIType::melee;
		header_img = MeleeImage;
		break;
	case 2:
		type = EIIType::ranged;
		header_img = RangedImage;
		break;
	case 3:
		type = EIIType::armor;
		header_img = ArmorImage;
		break;
	default:
		type = EIIType::etc;
		header_img = GeneralImage;
		UE_LOG(LogTemp, Error, TEXT("unknown curent_page in inventory widget: %d"), current_page)
		break;
	}

	if (header_img && CategoryImage)
		CategoryImage->SetBrushFromTexture(header_img);

	bool valid = false;
	int32 w, h;
	FString path;

	UInventoryItemWidget* item_widget=0;
	TArray <FInventoryItemBase*> items = inv_component->GetAllItemsOfType(type);
	for (FInventoryItemBase* item : items)
	{
		item_widget = WidgetTree->ConstructWidget<UInventoryItemWidget>(inventory_item_widget_bp);
		item_widget->SetParent(this);
		item_widget->id = item->id;

		current_widgets.Add(item_widget);

		valid = false;
		path = base_icon_path + item->icon_name + ".png";
		UTexture2D* icon = URuntimeUtils::LoadTexture2D_FromFile(path, EImageFormat::PNG, valid, w, h);
		//UE_LOG(LogTemp, Error, TEXT("UTexture2D: %d"), icon)

		item_widget->SetFore(icon);
        item_widget->SetCounter(item->count);

		InventoryBox->AddChild(item_widget);
	}
	
	if (items.Num())
	{
		UInventoryItemWidget* wid = Cast<UInventoryItemWidget>(InventoryBox->GetChildAt(0));
		if (wid)
			ClickItem(wid);
	}

	//node_widget->SetName(name);
	//node_widget->SetText(text);

	//FVector2D v = Dialog->GetDesiredSize();
	//Dialog->SetScrollOffset(v.Y);
}

void UInventoryWidget::UpdateBarterPage()
{
    if (!LootBox)
		return;

	LootBox->ClearChildren();

	if (!inv_component)
		return;

	if (!inventory_item_widget_bp)
		return;

	current_widgets.Empty();

	bool valid = false;
	int32 w, h;
	FString path;
	
    FInventoryItemBase* item=0;
	UInventoryItemWidget* item_widget=0;
	bool item_added = false;
    for (int i=0;i<barter_inv->Num();++i)
    {
		FLiteInvItem& lite_item = (*(barter_inv))[i];

        item = inv_component->GetItemFromDB(lite_item.id);
		
			if (!item)
            continue;
        
        item_widget = WidgetTree->ConstructWidget<UInventoryItemWidget>(inventory_item_widget_bp);
		item_widget->SetParent(this);
		item_widget->id = item->id;

		current_widgets.Add(item_widget);
        
        valid = false;
		path = base_icon_path + item->icon_name + ".png";
		UTexture2D* icon = URuntimeUtils::LoadTexture2D_FromFile(path, EImageFormat::PNG, valid, w, h);

		item_widget->SetFore(icon);
        item_widget->SetCounter(lite_item.amount);

		LootBox->AddChild(item_widget);
		item_added = true;
    }
	
	if (item_added)
	{
		UInventoryItemWidget* wid = Cast<UInventoryItemWidget>(LootBox->GetChildAt(0));
		if (wid)
			ClickItem(wid);
	}
}

void UInventoryWidget::ClickLeftButton()
{
	current_page--;

	if (current_page < 0)
		current_page = 3;

	UpdatePage();
}

void UInventoryWidget::ClickRightButton()
{
	current_page++;

	if (current_page > 3)
		current_page = 0;

	UpdatePage();
}

void UInventoryWidget::ClickUseButton()
{
    if (!inv_component)
		return;
    
    if (!current_widget)
        return;
    
    if (isBarter)
    {
        int ind=-1;
        for (int i=0;i<barter_inv->Num();++i)
        {
            if ((*(barter_inv))[i].id == current_widget->id)
            {
                ind = i;
                if ((*(barter_inv))[i].amount-1 >= 1)
                    ind = -1;
                break;
            }
        }
        if (ind != -1)
            barter_inv->RemoveAt(ind);
        
        inv_component->AddItem(current_widget->id);
        
        UpdateBarterPage();
    }
    else
    {
        // use, equip
		FInventoryItemBase* item = inv_component->GetItem(current_widget->id);
		if (!item)
			return;

		AMainCharacter* owner = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!owner)
			return;

		EIIType type = item->type;
		if (type == EIIType::ranged)
		{
			//FRangedItem* ri = static_cast<FRangedItem*>(item);
			//if (ri)

			owner->EquipRangedWeapon(dynamic_cast<FRangedItem*>(item));
		}
    }
}

void UInventoryWidget::ClickDropButton()
{
    
}

void UInventoryWidget::ClickExitButton()
{
    AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
		controller->EscapePressed();
}