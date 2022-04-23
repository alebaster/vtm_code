#include "DoorActor.h"

#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"

#include "NPC/EnemyCharacter.h"
#include "Character/MainCharacter.h"
#include "Lua/LuaComponent.h"

ADoorActor::ADoorActor()
{
	PrimaryActorTick.bCanEverTick = true;

	trigger_box = CreateDefaultSubobject<UBoxComponent>(TEXT("trigger_box"));
	trigger_box->SetupAttachment(RootComponent);

	key_id = "";
}

void ADoorActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (trigger_box)
		trigger_box->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnOverlapBegin);
	//mesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	forward = GetActorRightVector();

	TArray<USceneComponent*> all_components;
	GetComponents(all_components);
	for (auto c : all_components)
	{
		if (c->GetName() == "left_root")
			left_root_component = c;
		if (c->GetName() == "right_root")
			right_root_component = c;
	}

	if (!left_root_component && !right_root_component)
		UE_LOG(LogTemp, Error, TEXT("both door components are null"));

	AMainCharacter* character = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (character)
	{
		lua = Cast<ULuaComponent>(character->GetComponentByClass(ULuaComponent::StaticClass()));
		if (!lua)
			UE_LOG(LogTemp, Error, TEXT("cannot get lua_component in door actor"))
	}
	else
		UE_LOG(LogTemp, Error, TEXT("cannot get character in door actor"))
}

void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Rotate(DeltaTime);
}

//bool ADoorActor::IsAvaible()
//{
//
//}

UTexture2D* ADoorActor::GetActionImage()
{ 
	UTexture2D* tex = ActionImage;

	if (lua && key_id!="" && locked)
	{
		FString str = "HasItem(\"" + key_id + "\")";
		//FString str = LUA_EVAL("HasItem", key_id);
		if (!lua->ExecuteStringWBoolResult(str)) // no key
			tex = DenyImage;
	}

	return tex;
}

int ADoorActor::Use(AActor* who)
{
	// need key?
	if (locked)
	{
		if (lua && key_id != "")
		{
			FString str = "HasItem(\"" + key_id + "\")";
			//FString str = LUA_EVAL("HasItem", key_id);
			if (lua->ExecuteStringWBoolResult(str)) // has key
				; // open the door
			else // no key
			{
				AMainCharacter* character = Cast<AMainCharacter>(who);
				if (character)
					return character->StartLockpicking(this);
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("cannot cast who to AMainCharacter in Use() of DoorActor"));
					return 0;
				}
			}
		}
	}

	b_need_rotation = true;

	if (left_root_component)
		original_rotation_left = left_root_component->RelativeRotation;

	if (right_root_component)
		original_rotation_right = right_root_component->RelativeRotation;

	//start_time = GetWorld()->GetRealTimeSeconds();
	start_time = 0.0f;
	
	if (!state)
		UGameplayStatics::PlaySound2D(GetWorld(), OpenSound);

	FVector user_dir = who->GetActorForwardVector();
	FVector this_dir = forward;
	user_dir.Normalize();
	float dot = FVector::DotProduct(user_dir,this_dir);
	
	angle_left = dot<0? -90:90;
	angle_right = dot<0? 90:-90;

	who_actor = who;

	if (!state) // right now it closed but going to open
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADoorActor::AutoClose, 1.0f, true);
	else 
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	return 0;
}

void ADoorActor::Rotate(float DeltaTime)
{
	if (!b_need_rotation)
		return;
	
	float angle_l = state? 0.0f : angle_left;
	float angle_r = state? 0.0f : angle_right;

	FRotator target_rotation_l = FRotator(0,angle_l,0);
	FRotator target_rotation_r = FRotator(0,angle_r,0);

	//float distance = speed*(GetWorld()->GetRealTimeSeconds()-start_time);
	start_time += DeltaTime;
	float distance = speed * (start_time);
	float alpha = distance/90;
	
	if (left_root_component)
		left_root_component->SetRelativeRotation(FMath::Lerp(original_rotation_left, target_rotation_l, alpha));

	if (right_root_component)
		right_root_component->SetRelativeRotation(FMath::Lerp(original_rotation_right, target_rotation_r, alpha));
	
	if (FMath::Abs(distance-90) < 5)
	{
		if (left_root_component)
			left_root_component->SetRelativeRotation(target_rotation_l);

		if (right_root_component)
			right_root_component->SetRelativeRotation(target_rotation_r);

		b_need_rotation = false;
		state = !state;
	
		if (!state)
			UGameplayStatics::PlaySound2D(GetWorld(), CloseSound);
	}
}

void ADoorActor::AutoClose()
{
	if (who_actor)
	{
		float distance = FVector::Dist(who_actor->GetActorLocation(), this->GetActorLocation());
		//UE_LOG(LogTemp, Error, TEXT("dist %d"), who_actor);

		float distance2 = 1000.0f;
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(this, ACharacter::StaticClass(), actors);
		for (auto& a : actors)
		{
			float distance3 = FVector::Dist(a->GetActorLocation(), this->GetActorLocation());
			if (distance3 < distance2)
				distance2 = distance3;
		}

		if (distance > 150 && distance2 > 100)
		{
			Use(who_actor);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		}
	}
	else
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADoorActor::AutoClose, 1.0f, true);
}

void ADoorActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (locked)
		return;

	AEnemyCharacter* enemy = Cast<AEnemyCharacter>(OtherActor);
	if (enemy)
		Use(OtherActor);
}

void ADoorActor::PlayLockpicking()
{
	if (LockpickingSound)
		UGameplayStatics::PlaySound2D(GetWorld(), LockpickingSound);
}

void ADoorActor::PlayLockpickingSuccess()
{
	if (LockpickingSound)
		UGameplayStatics::PlaySound2D(GetWorld(), LockpickingSuccessSound);
}

void ADoorActor::PlayLockpickingFail()
{
	if (LockpickingSound)
		UGameplayStatics::PlaySound2D(GetWorld(), LockpickingFailSound);
}