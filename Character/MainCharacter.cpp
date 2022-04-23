#include "MainCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"

#include "Actors/StreamLevelTriggerActor.h"
#include "Actors/DoorActor.h"
#include "VtmPlayerController.h"
#include "CharacterAnimInstance.h"
#include "HandsActor.h"

#include "Core/RuntimeUtils.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//FollowCamera->SetupAttachment((USceneComponent*)GetMesh(), "head_socket");

	//CameraBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraBox"));
	//CameraBox->SetupAttachment(FollowCamera);

	//HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	//HandsMesh->SetupAttachment(FollowCamera);
	////HandsMesh->SetHiddenInGame(true);
	//HandsMesh->SetVisibility(false,true);
	//
	//LockPickMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LockPickMesh"));
	//LockPickMesh->SetupAttachment(FollowCamera);
	//LockPickMesh->SetHiddenInGame(true);
	//
	//WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	//WeaponMesh->SetupAttachment(FollowCamera);
	//WeaponMesh->SetHiddenInGame(true);

	//WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	//WeaponBox->SetupAttachment(GetMesh());

	// components
	dialog_manager = CreateDefaultSubobject<UDialogManager>(TEXT("DialogManager"));
	if (!dialog_manager)
		UE_LOG(LogTemp, Error, TEXT("dialog_manager is null"))

	wod_manager = CreateDefaultSubobject<UWodComponent>(TEXT("WodManager"));
	if (!wod_manager)
		UE_LOG(LogTemp, Error, TEXT("wod_manager is null"))

	quest_manager = CreateDefaultSubobject<UQuestManager>(TEXT("QuestManager"));
	if (!quest_manager)
		UE_LOG(LogTemp, Error, TEXT("quest_manager is null"))

	//hands_manager = CreateDefaultSubobject<UHandsController>(TEXT("HandsManager"));
	//if (!hands_manager)
	//	UE_LOG(LogTemp, Error, TEXT("hands_manager is null"))

	lua_component = CreateDefaultSubobject<ULuaComponent>(TEXT("LuaComponent"));
	if (!lua_component)
		UE_LOG(LogTemp, Error, TEXT("lua_component is null"))

	Crosshair_3D = CreateDefaultSubobject<UDecalComponent>("Crosshair_3D");
	Crosshair_3D->SetupAttachment(RootComponent);
	Crosshair_3D->SetVisibility(false);

	if (!hud_widget_bp)
	{
		ConstructorHelpers::FClassFinder<UMainHUD> UMainHUDClass(TEXT("WidgetBlueprint'/Game/UI/MainHUD_widget'"));
		if (UMainHUDClass.Succeeded())
			hud_widget_bp = UMainHUDClass.Class;
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//CameraBox = Cast<UBoxComponent>(GetComponentByClass(UBoxComponent::StaticClass()));
	if (GetCapsuleComponent())
	{
		//CameraBox->SetupAttachment(FollowCamera);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnOverlapBegin);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AMainCharacter::OnOverlapEnd);
	}

	if (b_camera_state==0)
	{
		GetMesh()->SetVisibility(false);
		CameraBoom->bDoCollisionTest = false;
	}

	// animation
	anim_inst = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (!anim_inst)
		UE_LOG(LogTemp, Error, TEXT("anim_inst in maincharacter is null"))

	// setup 3d crosshair
	if (Crosshair_3D_decal)
	{
		Crosshair_3D->SetDecalMaterial(Crosshair_3D_decal);
		Crosshair_3D->DecalSize = FVector(16.0f, 16.0f, 16.0f);
		//Crosshair_3D->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Crosshair_3D_decal is null"))

	if (hud_widget)
		hud_widget->ShowCrosshair();
    
	// spawn hands
    if (hands_actor_bp)
    {
		FTransform t;
		//t.SetRotation(FQuat(FRotator(0.0f,0.0f,-90.0f)));
		//t.SetLocation();
		FActorSpawnParameters sp;
		sp.Name = "HandsActor";
		sp.Owner = this;
		hands_actor = Cast<AHandsActor>(GetWorld()->SpawnActor(hands_actor_bp.GetDefaultObject()->GetClass(),&t,sp));
		if (hands_actor)
			hands_actor->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
		//hands_actor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		else
			UE_LOG(LogTemp, Error, TEXT("cant spawn hands actor"));
    }
	else
		UE_LOG(LogTemp, Error, TEXT("hands_actor_bp is null"));

	if (lua_component && dialog_manager)
		dialog_manager->SetLua(lua_component);

	if (lua_component && quest_manager)
		quest_manager->SetLua(lua_component);

	GetCharacterMovement()->MaxWalkSpeed = 200;
}

void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	APlayerController* PlayerController = Cast<APlayerController>(NewController);

	if (hud_widget_bp)
		hud_widget = CreateWidget<UMainHUD>(PlayerController, hud_widget_bp);
	else
		UE_LOG(LogTemp, Error, TEXT("hud_widget_bp is null"));

	if (!hud_widget)
		UE_LOG(LogTemp, Error, TEXT("hud widget not created"))
	else
		hud_widget->Show();
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveCamera(DeltaTime);

	MoveCameraToCrouch(DeltaTime);

	Raycast();

	MoveCameraToActor(DeltaTime);
}

void AMainCharacter::MoveCameraTo(const FTransform& transform)
{
	//UE_LOG(LogTemp, Error, TEXT("use_actor_transform: %s"), *transform.GetLocation().ToString())
	//UE_LOG(LogTemp, Error, TEXT("use_actor_transform: %s"), *transform.GetRotation().ToString())

	use_actor_transform = transform;
	original_transform = FollowCamera->GetComponentTransform();

	b_camera_in_use = true;

	//CameraControlPawn(false);

	//UE_LOG(LogTemp, Error, TEXT("original rot: %s"), *FollowCamera->GetComponentRotation().ToString())
	//UE_LOG(LogTemp, Error, TEXT("target rot: %s"), *transform.GetRotation().ToString())
	//CameraBoom->bUsePawnControlRotation = false;
	//FollowCamera->bUsePawnControlRotation = false;
}

void AMainCharacter::Raycast()
{
	float range = 0.0f;
	FVector StartLocation = FollowCamera->GetComponentLocation();
	if (b_camera_state == 0) // fps
		range = fps_raycast_range;
	else
		range = tps_raycast_range;

	FVector EndLocation = StartLocation + (FollowCamera->GetForwardVector() * range);;

	FHitResult RaycastHit;

	FCollisionQueryParams CQP;
	CQP.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(RaycastHit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CQP);

	if (b_camera_state)
	{
		FQuat SurfaceRotation = RaycastHit.ImpactNormal.ToOrientationRotator().Quaternion();
		Crosshair_3D->SetWorldLocationAndRotation(RaycastHit.Location, SurfaceRotation);

		b_weapon_near_wall = false;
	}
	if (!b_camera_state && b_weapon_equiped) // fps only
	{
		FHitResult AnotherRaycastHit;
		FVector AnotherEndLocation = StartLocation + (FollowCamera->GetForwardVector()*40);
		GetWorld()->LineTraceSingleByChannel(AnotherRaycastHit, StartLocation, AnotherEndLocation, ECollisionChannel::ECC_Visibility, CQP);
		if (AnotherRaycastHit.bBlockingHit)
			b_weapon_near_wall = true;
		else
			b_weapon_near_wall = false;
		
	}

	AActor* actor = RaycastHit.GetActor();
	if (!actor)
	{
		raycasted_component = 0;

		if (hud_widget)
		{
			hud_widget->HideUseTex();
			hud_widget->SetCrosshairDefault();
		}

		return;
	}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("hit actor %s"), *actor->GetName())

	// find usable component
	IUsableActor* usable_component = 0;

	float dist = FVector::Dist(GetActorLocation(), actor->GetActorLocation());
	//UE_LOG(LogTemp, Error, TEXT("dist %f"),dist)
	//UE_LOG(LogTemp, Error, TEXT("tps_interaction_range %f"), tps_interaction_range)

	if (dist <= tps_interaction_range) // ignore long range raycast
	{
		usable_component = Cast<IUsableActor>(actor);
		if (!usable_component) // not actor but component
		{
			TArray<UActorComponent*> all_components;
			actor->GetComponents(all_components);
			//UE_LOG(LogTemp, Error, TEXT("components num: %d"), all_components.Num())
			for (auto c : all_components)
			{
				usable_component = Cast<IUsableActor>(c);
				if (usable_component)
					break;
			}
		}
	}
	//IUsableActor* usable_component = Cast<IUsableActor>(RaycastHit.GetActor()->GetComponentByClass(TSubclassOf <IUsableActor>));

	if (usable_component)
	{
		if (raycasted_component == Cast<UObject>(usable_component))
			return;

		if (!hud_widget)
			return;

		raycasted_component = Cast<UObject>(usable_component);
		
		hud_widget->SetUseTex(usable_component->GetActionImage());
		hud_widget->ShowUseTex();
		hud_widget->SetCrosshairGreen();
		UE_LOG(LogTemp, Warning, TEXT("hit usable actor"));
	}
	else
	{
		raycasted_component = 0;
		if (hud_widget)
		{
			hud_widget->HideUseTex();
			hud_widget->SetCrosshairDefault();
		}
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMainCharacter::JumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMainCharacter::JumpReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAction("MoveForward", IE_Pressed, this, &AMainCharacter::MoveForwardPressed);
	PlayerInputComponent->BindAction("MoveBack", IE_Pressed, this, &AMainCharacter::MoveBackPressed);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &AMainCharacter::MoveRightPressed);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &AMainCharacter::MoveLeftPressed);

	PlayerInputComponent->BindAction("MoveForward", IE_Released, this, &AMainCharacter::MoveForwardReleased);
	PlayerInputComponent->BindAction("MoveBack", IE_Released, this, &AMainCharacter::MoveBackReleased);
	PlayerInputComponent->BindAction("MoveRight", IE_Released, this, &AMainCharacter::MoveRightReleased);
	PlayerInputComponent->BindAction("MoveLeft", IE_Released, this, &AMainCharacter::MoveLeftReleased);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMainCharacter::WalkPressed);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMainCharacter::WalkReleased);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::CrouchPressed);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAction("ChangeView", IE_Pressed, this, &AMainCharacter::SwitchCamera);

	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AMainCharacter::MeleePressed);
	PlayerInputComponent->BindAction("Ranged", IE_Pressed, this, &AMainCharacter::RangedPressed);

	// weapons
	PlayerInputComponent->BindAction("HideWeapon",   IE_Pressed, this, &AMainCharacter::HideWeaponPressed);
	PlayerInputComponent->BindAction("FireWeapon",   IE_Pressed, this, &AMainCharacter::FireWeaponPressed);
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, this, &AMainCharacter::ReloadWeaponPressed);
}	

int AMainCharacter::Use()
{
	if (b_input_blocked)
		return 0;

	if (!raycasted_component)
		return 0;

	IUsableActor* usable_component = Cast<IUsableActor>(raycasted_component);
	if (!usable_component)
		return 0;

	hud_widget->HideUseTex();
    
    return usable_component->Use(this);
}

void AMainCharacter::EndUse()
{
    IUsableActor* usable_component = Cast<IUsableActor>(raycasted_component);
	if (!usable_component)
		return;

	hud_widget->ShowUseTex();
    
    usable_component->EndUse(this);
}

int AMainCharacter::StartLockpicking(ADoorActor* door)
{
	if (!door)
		return 0;

	if (!hud_widget)
		return 0;

	if (!lua_component)
		return 0;

	//FString str = "HasItem(\\\"lockpick\\\")";
	FString str = LUA_EVAL_S("HasItem", "lockpick");
	if (!lua_component->ExecuteStringWBoolResult(str))
	{
		hud_widget->ShowLockNeed();
		return 0;
	}

	UArrowComponent* arr = Cast<UArrowComponent>(door->GetComponentByClass(UArrowComponent::StaticClass()));
	if (!arr)
		return 0;

	FTransform transform = arr->GetComponentTransform();
	transform.SetRotation(transform.GetRotation().Inverse());
	//transform.SetLocation(transform.GetLocation()+door->GetActorRightVector()*50);
	MoveCameraTo(transform);
	hud_widget->StartLockpicking(door);
    if (hands_actor)
		hands_actor->EquipLockpick();

	return 3;
}

void AMainCharacter::EndLockpicking()
{
	ReturnCameraToFPS();
    
    if (hands_actor)
		hands_actor->EndLockpick();
}

void AMainCharacter::ReturnCameraToFPS()
{
	if (use_actor_transform.GetLocation() != FVector(0, 0, 0))
	{
		use_actor_transform.SetLocation(FVector(0, 0, 0));
		b_camera_in_use = true;
	}
}

void AMainCharacter::CharacterPressed()
{

}

void AMainCharacter::InventoryPressed()
{

}

void AMainCharacter::JournalPressed()
{

}

void AMainCharacter::HideWeaponPressed()
{
	if (hands_actor)
		hands_actor->hide_pressed = true;

	// update HUD
	if (hud_widget)
		hud_widget->HideWeaponImage();
}

void AMainCharacter::FireWeaponPressed()
{
	if (hands_actor)
		hands_actor->fire_pressed = true;
}

void AMainCharacter::ReloadWeaponPressed()
{
	if (hands_actor)
		hands_actor->reload_pressed = true;
}

void AMainCharacter::MoveForward(float Value)
{
	//if (!InputEnabled())
	//	return;

	if (b_weapon_near_wall && Value > 0)
		return;

	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		//if (anim_inst)
		//{
		//	anim_inst->SetRunning();
		//}
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);

		//if (anim_inst)
		//{
		//	anim_inst->SetRunning();
		//}
	}
}

void AMainCharacter::JumpPressed()
{
	Super::Jump();

	//const FRotator Rotation = Controller->GetControlRotation();
	//const FRotator YawRotation(0, Rotation.Yaw, 0);
	//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
	//AddMovementInput(Direction, 300);
	//SetActorLocation(GetActorUpVector() * 300.0f);
	//GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	//AddMovementInput(GetActorUpVector(), 0.1f);

	if (anim_inst)
		anim_inst->JumpPressed();
}

void AMainCharacter::JumpReleased()
{
	//GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	Super::StopJumping();
}

void AMainCharacter::JumpEndBeginNotyfyCallback()
{
	//GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void AMainCharacter::JumpEndEndNotyfyCallback()
{
	//GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMainCharacter::Turn(float Value)
{
	if (b_input_blocked)
		return;

	AddControllerYawInput(Value);
}

void AMainCharacter::LookUp(float Value)
{
	if (b_input_blocked)
		return;

	AddControllerPitchInput(Value);
}

void AMainCharacter::SwitchCamera()
{
	if (b_input_blocked)
		return;

	b_camera_state = b_camera_state==0? 3 : 2;

	if (!hud_widget)
		return;
	if (b_camera_state == 2) // to fps
	{
		hud_widget->ShowCrosshair();
		HideCrosshair3D();
	}
	else // to tps
	{
		hud_widget->HideCrosshair();
		ShowCrosshair3D();
	}
}

void AMainCharacter::MoveCamera(float delta)
{
	if (b_camera_state <= 1)
		return;

	//float speed = 5*delta;
	float new_len = 0.0f;
	float new_off = 0.0f;

	float speed = 3*delta;
	current_lerp_time += speed;
	float a = current_lerp_time / lerp_time;

	if (b_camera_state == 2) // to fps
	{
		new_len = 0.0f;
		//CameraBoom->bUsePawnControlRotation = false;
	}
	else if (b_camera_state == 3) // to tps
	{
		new_len = 100.0f;
		//GetMesh()->SetVisibility(true);
		new_off = 15;
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bDoCollisionTest = true;
	}

	CameraBoom->TargetArmLength = FMath::Lerp(CameraBoom->TargetArmLength, new_len, a);
	CameraBoom->TargetOffset.Z = FMath::Lerp(CameraBoom->TargetOffset.Z, new_off, a);
	//UE_LOG(LogTemp, Error, TEXT("her %f"), a);
	//GetMesh()->SetVisibility(false);
	// near end
	if (CameraBoom->TargetArmLength<20 && b_camera_state==2) // to fps
		GetMesh()->SetVisibility(false);

	if (CameraBoom->TargetArmLength>20 && b_camera_state==3) // to tps
		GetMesh()->SetVisibility(true);

	// end to fps
	if (CameraBoom->TargetArmLength < 1 && b_camera_state == 2)
	{
		current_lerp_time = 0.0f;
		CameraBoom->TargetArmLength = new_len;

		b_camera_state = 0;
		//GetMesh()->SetVisibility(false);
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bDoCollisionTest = false;
		CameraBoom->TargetOffset.Z = 0;
	}

	// end to tps
	if (CameraBoom->TargetArmLength > 99 && b_camera_state == 3)
	{
		current_lerp_time = 0.0f;
		CameraBoom->TargetArmLength = new_len;
		b_camera_state = 1;
		CameraBoom->TargetOffset.Z = 15;
	}
}

void AMainCharacter::MoveCameraToCrouch(float delta)
{
	if (b_camera_state >= 2)
		return;

	if (b_crouch <= 1)
		return;

	float speed = 3 * delta;
	current_lerp_time += speed;
	float a = current_lerp_time / lerp_time;

	int offset = 0;

	if (b_crouch == 3) // to crouch
		offset = 0;
	else if (b_crouch == 2) // to stand
		offset = 40;

	//CameraBoom->SetRelativeLocation(FVector(0,0,offset));
	//CameraBoom->RelativeLocation = FMath::Lerp(CameraBoom->RelativeLocation, FVector(0, 0, offset),a);
	CameraBoom->SetRelativeLocation(FMath::Lerp(CameraBoom->RelativeLocation, FVector(0, 0, offset), a));

	if (FVector::Dist(CameraBoom->RelativeLocation, FVector(0, 0, offset)) < 1)
	{
		CameraBoom->RelativeLocation = FVector(0, 0, offset);
		current_lerp_time = 0.0f;

		// end to crouch
		if (b_crouch == 3)
			b_crouch = 0;

		// end to stand
		if (b_crouch == 2)
			b_crouch = 1;

		//UE_LOG(LogTemp, Error, TEXT("end crouching"));
	}
}

void AMainCharacter::MoveForwardPressed()
{
	if (anim_inst)
		anim_inst->WPress();
}

void AMainCharacter::MoveBackPressed()
{
	if (anim_inst)
		anim_inst->SPress();
}

void AMainCharacter::MoveRightPressed()
{
	if (anim_inst)
		anim_inst->DPress();
}

void AMainCharacter::MoveLeftPressed()
{
	if (anim_inst)
		anim_inst->APress();
}

void AMainCharacter::MoveForwardReleased()
{
	if (anim_inst)
		anim_inst->WRelease();
}

void AMainCharacter::MoveBackReleased()
{
	if (anim_inst)
		anim_inst->SRelease();
}

void AMainCharacter::MoveRightReleased()
{
	if (anim_inst)
		anim_inst->DRelease();
}

void AMainCharacter::MoveLeftReleased()
{
	if (anim_inst)
		anim_inst->ARelease();
}

void AMainCharacter::WalkPressed()
{
	if (anim_inst)
	{
		anim_inst->WalkPressed();
		GetCharacterMovement()->MaxWalkSpeed = 80;
	}

}

void AMainCharacter::WalkReleased()
{
	if (anim_inst)
	{
		anim_inst->WalkReleased();
		GetCharacterMovement()->MaxWalkSpeed = 200;
	}
}

void AMainCharacter::CrouchPressed()
{
	if (anim_inst)
	{
		anim_inst->CrouchPressed();
		if (anim_inst->isCrouching)
			GetCharacterMovement()->MaxWalkSpeed = 50;
		else
			GetCharacterMovement()->MaxWalkSpeed = 200;

		b_crouch = anim_inst->isCrouching? 3 : 2;
	}
}

void AMainCharacter::EquipRangedWeapon(FRangedItem* weapon)
{
	if (!weapon)
		return;

	if (!hands_actor)
		return;

	auto f = ranged_weapons.Find(weapon->id);
	if (!f)
	{
		FString path = "/Game/Blueprint/weapons/";
		UClass* bp = URuntimeUtils::GetBlueprint<ARangedWeaponActor>(path, weapon->id);
		if (!bp)
			UE_LOG(LogTemp, Error, TEXT("cannot get ranged weapon blueprint with id: %s"), *weapon->id);

		FTransform t;
		//st.SetRotation(FQuat(FRotator(0.0f, 0.0f, -90.0f)));
		//t.SetLocation();
		FActorSpawnParameters sp;
		//sp.Name = *weapon->id;
		sp.Owner = hands_actor;
		ARangedWeaponActor* obj = Cast<ARangedWeaponActor>(GetWorld()->SpawnActor(bp->GetDefaultObject()->GetClass(), &t, sp));
		//ARangedWeaponActor* obj = NewObject<ARangedWeaponActor>(this, *weapon->id, EObjectFlags::RF_NoFlags, bp->GetDefaultObject());
		if (obj)
		{
			//obj->AttachToComponent(FollowCamera, FAttachmentTransformRules::KeepRelativeTransform);
			obj->item_descr = weapon;
			obj->AttachToActor(hands_actor, FAttachmentTransformRules::KeepRelativeTransform);
			f = &ranged_weapons.Add(weapon->id, obj);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("failed to spawn weapon with id: %s"), *weapon->id);
			return;
		}
	}

	last_ranged_weapon = weapon;

	hands_actor->EquipRangedWeapon(*f);
	b_weapon_equiped = true;

	// update HUD
	if (hud_widget)
	{
		bool valid = false;
		int32 w, h;
		FString path = FPaths::ProjectContentDir() + "RuntimeResources/item_image/" + weapon->icon_name + ".png";
		UTexture2D* icon = URuntimeUtils::LoadTexture2D_FromFile(path, EImageFormat::PNG, valid, w, h);
		hud_widget->ShowWeaponImage(icon);
	}
}

void AMainCharacter::MeleePressed()
{
	
}

void AMainCharacter::RangedPressed()
{
	// equip last ranged weapon
	if (last_ranged_weapon)
		EquipRangedWeapon(last_ranged_weapon);
}

void AMainCharacter::MoveCameraToActor(float delta)
{
	if (!b_camera_in_use)
		return;

	bool loc_end = false;
	bool rot_end = false;

	float speed = 3 * delta;
	current_lerp_time += speed;
	float a = current_lerp_time/lerp_time;

	FTransform destination = use_actor_transform.GetLocation()==FVector(0,0,0)? original_transform : use_actor_transform;
	FRotator r = destination.GetRotation().Rotator();
	r.Roll = 0.0f;
	destination.SetRotation(FQuat(r));

	FollowCamera->SetWorldLocation(FMath::Lerp(FollowCamera->GetComponentLocation(), destination.GetLocation(), a));
	FollowCamera->SetWorldRotation(FMath::Lerp(FollowCamera->GetComponentRotation(), destination.GetRotation().Rotator(), a));

	if (FVector::Distance(FollowCamera->GetComponentLocation(), destination.GetLocation()) < 0.1f)
	{
		loc_end = true;
		FollowCamera->SetWorldLocation(destination.GetLocation());
	}

	if (FollowCamera->GetComponentRotation().Equals(destination.GetRotation().Rotator(), 0.1))
	{
		FRotator r = destination.GetRotation().Rotator();
		r.Roll = 0.0f;
		FollowCamera->SetWorldRotation(r);
		rot_end = true;
	}

	if (loc_end && rot_end)
	{
		b_camera_in_use = false;
		current_lerp_time = 0.0f;

		// back to fps
		if (use_actor_transform.GetLocation() == FVector(0,0,0))
		{
			AVtmPlayerController* vpc = Cast<AVtmPlayerController>(GetController());
			if (vpc)
			{
				vpc->EnableControls();
				UE_LOG(LogTemp, Warning, TEXT("return controls to player"));
			}
		}
	}
}

// camera rotation remain fixed to pawn rotation
void AMainCharacter::CameraControlPawn(bool flag)
{
	//if (CameraBoom)
	//	CameraBoom->bUsePawnControlRotation = flag;
	if (FollowCamera)
		FollowCamera->bUsePawnControlRotation = flag;
}

void AMainCharacter::ShowCrosshair3D()
{
	if (Crosshair_3D)
		Crosshair_3D->SetVisibility(true);
}

void AMainCharacter::HideCrosshair3D()
{
	if (Crosshair_3D)
		Crosshair_3D->SetVisibility(false);
}

void AMainCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("OtherComp %d"), OtherActor == this);

	if (!bcutscene_activated)
	{
		if (OtherActor->GetName() == "CusceneTrigger")
		{
			UE_LOG(LogTemp, Warning, TEXT("cutscene!"));
			hud_widget->Hide();
			bcutscene_activated = true;
		}
	}

	// get rig of mesh while camera inside it
	if (b_camera_state == 1 || b_camera_state == 2)
	{
		//if (OtherActor == this && OtherComp == CameraBox)
		{
			//GetMesh()->SetVisibility(false);
		}
	}
	//else
	//{
	//	AStreamLevelTriggerActor* sltg = Cast<AStreamLevelTriggerActor>(OtherActor);
	//	if (sltg)
	//		sltg->enter();
	//}
}

void AMainCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("end overlap"));

	// get rig of mesh while camera inside it
	if (b_camera_state == 1 || b_camera_state == 2)
	{
		//if (OtherActor == this && OtherComp == CameraBox)
		{
			//GetMesh()->SetVisibility(true);
		}
	}
	//else
	//{
	//	AStreamLevelTriggerActor* sltg = Cast<AStreamLevelTriggerActor>(OtherActor);
	//	if (sltg)
	//		sltg->leave();
	//}
}

void AMainCharacter::OnCutsceneStart()
{

}

void AMainCharacter::OnCutsceneEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("cutscene end!"));

	hud_widget->Show();

	// tp
}