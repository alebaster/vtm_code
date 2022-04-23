#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ActorComponent.h"
#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"

#include "UI/MainHUD.h"
#include "Inventory/InventoryItem.h"
#include "Actors/UsableActor.h"
#include "Weapon/RangedWeaponActor.h"

#include "Quest/QuestManager.h"
#include "Dialog/DialogManager.h"
#include "Lua/LuaComponent.h"
#include "Character/WodComponent.h"

#include "MainCharacter.generated.h"

class ADoorActor;
class UCharacterAnimInstance;
class AHandsActor;

UCLASS(config=Game)
class VTM_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();

	UMainHUD* GetHud() { return hud_widget; }

	int Use();
	void EndUse();
	void ReturnCameraToFPS();
	void CameraControlPawn(bool flag);

	void EquipRangedWeapon(FRangedItem* weapon);

	int StartLockpicking(ADoorActor* door);
	void EndLockpicking();

	//void BlockInput(bool block);
	void MoveCameraTo(const FTransform& transform);

	//void WidgetEscape();

	FString GetPlayerName() { return PlayerName; }

	void JumpEndBeginNotyfyCallback();
	void JumpEndEndNotyfyCallback();

	UQuestManager* GetQuestManager() { return quest_manager; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 
	UPROPERTY(EditAnywhere, Category = Mics, meta = (AllowPrivateAccess = "true"))
	FString PlayerName="name";

	//UPROPERTY(EditAnywhere)
	//USkeletalMeshComponent* HandsMesh;
	//
	//UPROPERTY(EditAnywhere)
	//USkeletalMeshComponent* LockPickMesh;
	//
	//UPROPERTY(EditAnywhere)
	//USkeletalMeshComponent* WeaponMesh;

	//UPROPERTY(EditAnywhere)
	//class UBoxComponent* WeaponBox;

	UPROPERTY()
	UDecalComponent* Crosshair_3D;

	UPROPERTY(EditAnywhere, Category = Default)
	UMaterial* Crosshair_3D_decal;

	void ShowCrosshair3D();
	void HideCrosshair3D();

	// camera
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom=0;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera=0;

	//UPROPERTY(EditDefaultsOnly, Instanced, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UBoxComponent* CameraBox=0;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void OnCutsceneStart();

	UFUNCTION(BlueprintCallable)
	void OnCutsceneEnd();
	
	uint8_t b_camera_state=0;	//  0 - fps, 1 - tps, 2 - to fps, 3 - to tps
	bool b_camera_in_use=false;
	uint8_t b_crouch=0; // 0 - stand, 1 - crouch, 2 - to stand, 3 - to crouch

	bool bcutscene_activated = false;

	float lerp_time=3.0f;
	float current_lerp_time=0.0f;;
	void MoveCameraToActor(float delta);

	bool b_weapon_near_wall=false;

	// input
	bool b_input_blocked=false;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void JumpPressed();
	void JumpReleased();
	void Turn(float Value);
	void LookUp(float Value);
	void SwitchCamera();
	void MoveCamera(float delta);
	void MoveCameraToCrouch(float delta);
	void CharacterPressed();
	void InventoryPressed();
	void JournalPressed();

	// weapons
	void HideWeaponPressed();
	void FireWeaponPressed();
	void ReloadWeaponPressed();

	// animation
	UCharacterAnimInstance* anim_inst;

	void MoveForwardPressed();
	void MoveBackPressed();
	void MoveRightPressed();
	void MoveLeftPressed();
	void MoveForwardReleased();
	void MoveBackReleased();
	void MoveRightReleased();
	void MoveLeftReleased();
	void WalkPressed();
	void WalkReleased();
	void CrouchPressed();

	void MeleePressed();
	void RangedPressed();

	// UI
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainHUD> hud_widget_bp=0;
	UPROPERTY()
	UMainHUD* hud_widget = 0;

	// interaction
	UPROPERTY()
	float fps_raycast_range = 60.0f;

	UPROPERTY()
	float tps_raycast_range = 500.0f;

	UPROPERTY()
	float tps_interaction_range = 60.0f;

	FTransform original_transform;
	FTransform use_actor_transform;

	UPROPERTY()
	UObject* raycasted_component=0;

	UFUNCTION()
	void Raycast();

	bool b_weapon_equiped=false;

	// weapons
	UPROPERTY()
	TMap<FString,ARangedWeaponActor*> ranged_weapons;
	FRangedItem* last_ranged_weapon=0;

	// dialog
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UDialogManager* dialog_manager=0;

	// wod
	UPROPERTY()
	UWodComponent* wod_manager = 0;

	// quest
	UPROPERTY()
	UQuestManager* quest_manager=0;

	// hands
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHandsActor> hands_actor_bp=0;

	UPROPERTY()
	AHandsActor* hands_actor = 0;

	// lua
	UPROPERTY()
	ULuaComponent* lua_component=0;
};
