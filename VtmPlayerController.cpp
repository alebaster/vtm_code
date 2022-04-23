#include "VtmPlayerController.h"
#include "Character/MainCharacter.h"

#include "GameFramework/Character.h"

#include "UI/JournalWidget.h"
#include "Dialog/DialogManager.h"

AVtmPlayerController::AVtmPlayerController()
{
	// components
	inventory_component = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	if (!inventory_component)
		UE_LOG(LogTemp, Error, TEXT("inventory_component is null"))
}

void AVtmPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FLatentActionInfo LatentInfo;
	UGameplayStatics::LoadStreamLevel(GetWorld(), TEXT("/Game/maps/sm_hideout/sm_hideout"), true, true, LatentInfo);
}

void AVtmPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

    InputComponent->BindAction("Use", IE_Pressed, this, &AVtmPlayerController::Use);
    InputComponent->BindAction("Esc", IE_Pressed, this, &AVtmPlayerController::EscapePressed);
    
	InputComponent->BindAction("Character", IE_Pressed, this, &AVtmPlayerController::CharacterPressed);
	InputComponent->BindAction("Inventory", IE_Pressed, this, &AVtmPlayerController::InventoryPressed);
	InputComponent->BindAction("Journal", IE_Pressed, this, &AVtmPlayerController::JournalPressed);

	InputComponent->BindAction("Test", IE_Pressed, this, &AVtmPlayerController::TestPressed);
	InputComponent->BindAction("Test2", IE_Pressed, this, &AVtmPlayerController::TestPressed2);
}

void AVtmPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (mm_widget_bp)
		mm_widget = CreateWidget<UMainMultiWidget>(this, mm_widget_bp);
	if (!mm_widget)
		UE_LOG(LogTemp, Error, TEXT("main multi widget not created"));

	if (inv_widget_bp)
		inv_widget = CreateWidget<UInventoryWidget>(this, inv_widget_bp);
	if (!inv_widget)
		UE_LOG(LogTemp, Error, TEXT("inventory widget not created"));

	if (inventory_component)
	{
		//inventory_component->AddItem("lockpick");

		if (inv_widget)
			inv_widget->SetInventoryComponent(inventory_component);
	}

	if (!mm_widget)
		return;

	UJournalWidget* j = mm_widget->GetJournalWidget();
	if (!j)
	{
		UE_LOG(LogTemp, Error, TEXT("GetJournalWidget in OnPossess is null"));
		return;
	}

	AMainCharacter* mc = Cast<AMainCharacter>(InPawn);
	if (!mc)
	{
		UE_LOG(LogTemp, Error, TEXT("AMainCharacter in OnPossess is null"));
		return;
	}

	j->SetQuestManager(mc->GetQuestManager());
}

bool AVtmPlayerController::InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	// dialog
	if (last_use_type == 2)
	{
		if (EventType == EInputEvent::IE_Released)
		{
			AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
			if (!mc)
				return false;

			UDialogManager* dialog_manager = Cast<UDialogManager>(mc->GetComponentByClass(UDialogManager::StaticClass()));
			if (!dialog_manager)
				return false;

			INT16 n = 0;

			if (Key == "SpaceBar")
			{
				dialog_manager->Skip();
				return false;
			}

			if (Key == "One")
				n = 1;
			else if (Key == "Two")
				n = 2;
			else if (Key == "Three")
				n = 3;
			else if (Key == "Four")
				n = 4;
			else if (Key == "Five")
				n = 5;
			else if (Key == "Six")
				n = 6;
			else if (Key == "Seven")
				n = 7;
			else if (Key == "Eight")
				n = 8;
			else if (Key == "Nine")
				n = 9;

			if (n!=0)
				dialog_manager->ReactToAnswer(n);
		}
	}

	return Super::InputKey(Key, EventType, AmountDepressed, bGamepad);
}

void AVtmPlayerController::EnableControls()
{
	ShowMouseCursor(false);
	BlockMouseControl(false);
	BlockKeyboardControl(false);

	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	if (mc)
		mc->CameraControlPawn(true);
}

void AVtmPlayerController::ShowMouseCursor(bool show)
{
	bShowMouseCursor = show;
	bEnableClickEvents = show;
	bEnableMouseOverEvents = show;
}

void AVtmPlayerController::BlockMouseControl(bool block)
{
	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	if (!mc)
		return;

	block ? SetInputMode(FInputModeGameAndUI()) : SetInputMode(FInputModeGameOnly());
	//mc->CameraControlPawn(!block);
}

void AVtmPlayerController::BlockKeyboardControl(bool block)
{
	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	if (!mc)
		return;

	UE_LOG(LogTemp, Warning, TEXT("block keaboard: %d"),block)

	block ? mc->DisableInput(this) : mc->EnableInput(this);
}

void AVtmPlayerController::EscapePressed()
{
	if (last_use_type == 2)
		return;

	if (last_use_type == 4)
		EndUse();

    if (mm_widget)
    {
        if (mm_widget->OnScreen())
            mm_widget->Hide();
    }

    if (inv_widget)
    {
        if (inv_widget->OnScreen())
        {
            if (inv_widget->isBarter)
				EndUse();
            
            inv_widget->Hide();
        }
    }

	BlockKeyboardControl(false);
	BlockMouseControl(false);
	ShowMouseCursor(false);
}

void AVtmPlayerController::Use()
{
	if (inv_widget)
	{
		if (inv_widget->OnScreen())
			return;
	}

	if (mm_widget)
	{
		if (mm_widget->OnScreen())
			return;
	}

	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	if (!mc)
		return;

	// press use button while already use something
	if (last_use_type)
	{
		if (last_use_type == 1)
			EndUse();
		return;
	}

	last_use_type = mc->Use();
	UE_LOG(LogTemp, Warning, TEXT("last_use_type: %d"), last_use_type)
	
	if (!last_use_type) // instant
		return;

	if (last_use_type == 5)
		ShowMouseCursor(true);
	else
		ShowMouseCursor(false);

	// with camera move
	if (last_use_type == 2 ||
		last_use_type == 3 ||
		last_use_type == 4)
		mc->CameraControlPawn(false);

	BlockMouseControl(true);
	BlockKeyboardControl(true);
}

void AVtmPlayerController::EndUse()
{
	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	if (!mc)
		return ;

	int t = last_use_type;
	last_use_type = 0;

	switch (t)
	{
	case 1: // reuse or esc (notes)
		mc->EndUse();
		break;
	case 2: // command within (dialog)
		mc->ReturnCameraToFPS();
		mc->EndUse();
		return; // wait for camera move
		break;
	case 3: // auto (lockpicking)
		mc->EndLockpicking();
		return; // wait for camera move
		break;
	case 4: // command within or esc (hacking)
		mc->ReturnCameraToFPS();
		mc->EndUse();
		return; // wait for camera move
		break;
	case 5: // mouse click or esc (barter) [need mouse]
		if (inv_widget)
			inv_widget->isBarter = false;
		mc->EndUse();
		break;
	default:
		break;
	}

	EnableControls();
}

void AVtmPlayerController::CharacterPressed()
{
	if (last_use_type)
		return;

	if (!mm_widget)
		return;

	mm_widget->ShowCharacter();

	BlockKeyboardControl(mm_widget->OnScreen());
	BlockMouseControl(mm_widget->OnScreen());
	ShowMouseCursor(mm_widget->OnScreen());
}

void AVtmPlayerController::JournalPressed()
{
	if (last_use_type)
		return;

	if (!mm_widget)
		return;

	mm_widget->ShowJournal();

	BlockKeyboardControl(mm_widget->OnScreen());
	BlockMouseControl(mm_widget->OnScreen());
	ShowMouseCursor(mm_widget->OnScreen());
}

void AVtmPlayerController::InventoryPressed()
{
	if (last_use_type)
		return;

	if (!inv_widget)
		return;

    // ???
    if (inv_widget->isBarter)
        return;
    
	inv_widget->Show();

	BlockKeyboardControl(inv_widget->OnScreen());
	BlockMouseControl(inv_widget->OnScreen());
	ShowMouseCursor(inv_widget->OnScreen());
}

// from loot actor component
void AVtmPlayerController::StartBarter(TArray<FLiteInvItem>& inv)
{
    if (!inv_widget)
		return;
    
    inv_widget->isBarter = true;
    inv_widget->ShowBarter(inv);
    BlockMouseControl(true);
	ShowMouseCursor(true);
}

void AVtmPlayerController::TestPressed()
{
	//FLatentActionInfo LatentInfo;
	//UGameplayStatics::LoadStreamLevel(GetWorld(), TEXT("/Game/maps/sm_hub/sm_hub"), true, true, LatentInfo);
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/maps/sm_hub/sm_hub"));
}

void AVtmPlayerController::TestPressed2()
{
	AMainCharacter* mc = Cast<AMainCharacter>(GetCharacter());
	ULuaComponent* lua_component = Cast<ULuaComponent>(mc->GetComponentByClass(ULuaComponent::StaticClass()));
	if (lua_component)
	{
		
		FInventoryItemBase* item = lua_component->test();
		if (item)
		{
			UE_LOG(LogTemp, Warning, TEXT("1"));
			FRangedItem* ritem = static_cast<FRangedItem*>(item);
			if (ritem)
			{
				UE_LOG(LogTemp, Warning, TEXT("2"));
				mc->EquipRangedWeapon(ritem);
			}
		}
	}

	//FLatentActionInfo LatentInfo;
	//UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("/Game/maps/sm_hideout/sm_hideout"), LatentInfo, true);
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/maps/sm_hub/sm_hub"));
}
