#include "StreamLevelTriggerActor.h"

#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "Character/MainCharacter.h"

AStreamLevelTriggerActor::AStreamLevelTriggerActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AStreamLevelTriggerActor::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Error, TEXT("AStreamLevelTriggerActor BeginPlay"));
	
	TArray<USceneComponent*> all_components;
	GetComponents(all_components);
	for (auto c : all_components)
	{
		if (c->GetName() == "first_box_trigger")
			first_box = Cast<UBoxComponent>(c);
		if (c->GetName() == "second_box_trigger")
			second_box = Cast<UBoxComponent>(c);
	}

	if (!first_box || !second_box)
	{
		UE_LOG(LogTemp, Error, TEXT("some trigger box is null"));
		return;
	}


	first_box->OnComponentBeginOverlap.AddDynamic(this, &AStreamLevelTriggerActor::OnOverlapBegin_first);
	first_box->OnComponentEndOverlap.AddDynamic(this, &AStreamLevelTriggerActor::OnOverlapEnd_first);

	second_box->OnComponentBeginOverlap.AddDynamic(this, &AStreamLevelTriggerActor::OnOverlapBegin_second);
	second_box->OnComponentEndOverlap.AddDynamic(this, &AStreamLevelTriggerActor::OnOverlapEnd_second);
}

void AStreamLevelTriggerActor::enter()
{
	FString level = activated? first_level : second_level;

	if (!level.IsEmpty())
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*(FString("/Game/maps/") + level)), true, true, LatentInfo);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("level for loading is empty"));
}

void AStreamLevelTriggerActor::leave()
{
	FString level = activated? second_level : first_level;

	if (!level.IsEmpty())
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(*(FString("/Game/maps/") + level)), LatentInfo, true);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("level for unloading is empty"));

	activated = !activated;
}

//void AStreamLevelTriggerActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AStreamLevelTriggerActor::OnOverlapBegin_first(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
													int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* mc = Cast<AMainCharacter>(OtherActor);
	if (mc && mc->GetCapsuleComponent()==OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("overlap first"));

		in_first_box = true;

		if (in_second_box)
		{

		}
		else
		{
			FLatentActionInfo LatentInfo;
			FName level = FName(*(FString("/Game/maps/") + second_level));
			UGameplayStatics::LoadStreamLevel(GetWorld(), level, true, false, LatentInfo);

			ULevelStreaming* stream_level = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
			if (stream_level)
				stream_level->OnLevelLoaded.AddDynamic(this, &AStreamLevelTriggerActor::SecondLevelLoaded);
			else
				UE_LOG(LogTemp, Error, TEXT("stream_levelg is null"));
		}
	}
}

void AStreamLevelTriggerActor::OnOverlapEnd_first(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* mc = Cast<AMainCharacter>(OtherActor);
	if (mc && mc->GetCapsuleComponent() == OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("end overlap first"));

		in_first_box = false;
		if (second_actor_lazy.IsValid())
		{
			second_actor_lazy->SetActorHiddenInGame(false);
			second_actor_lazy->SetActorEnableCollision(true);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("second_actor_lazy is invalid"));

		if (in_second_box)
		{

		}
		else
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(*(FString("/Game/maps/") + second_level)), LatentInfo, true);
			//remove delegate?
		}
	}
}

void AStreamLevelTriggerActor::OnOverlapBegin_second(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
													 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* mc = Cast<AMainCharacter>(OtherActor);
	if (mc && mc->GetCapsuleComponent() == OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("overlap second"));
		in_second_box = true;

		if (in_first_box)
		{

		}
		else
		{
			FLatentActionInfo LatentInfo;
			FName level = FName(*(FString("/Game/maps/") + first_level));
			UGameplayStatics::LoadStreamLevel(GetWorld(), level, true, false, LatentInfo);
			
			ULevelStreaming* stream_level = UGameplayStatics::GetStreamingLevel(GetWorld(), level);
			if (stream_level)
				stream_level->OnLevelLoaded.AddDynamic(this, &AStreamLevelTriggerActor::FirstLevelLoaded);
		}
	}
}

void AStreamLevelTriggerActor::OnOverlapEnd_second(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* mc = Cast<AMainCharacter>(OtherActor);
	if (mc && mc->GetCapsuleComponent() == OtherComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("end overlap second"));

		in_second_box = false;
		if (first_actor_lazy.IsValid())
		{
			first_actor_lazy->SetActorHiddenInGame(false);
			first_actor_lazy->SetActorEnableCollision(true);
		}
		else
			UE_LOG(LogTemp, Error, TEXT("first_actor_lazy is invalid"));

		if (in_first_box)
		{

		}
		else
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(GetWorld(), FName(*(FString("/Game/maps/") + first_level)), LatentInfo, true);
			//remove delegate?
		}
	}
}

void AStreamLevelTriggerActor::FirstLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("FirstLevelLoaded"));

	if (first_actor_lazy.IsValid())
	{
		first_actor_lazy->SetActorHiddenInGame(true);
		first_actor_lazy->SetActorEnableCollision(false);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("first_actor_lazy is invalid"));

	//if (second_actor_lazy.IsValid())
	//{
	//	second_actor_lazy->SetActorHiddenInGame(false);
	//	second_actor_lazy->SetActorEnableCollision(true);
	//}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("second_actor_lazy is invalid"));
}

void AStreamLevelTriggerActor::SecondLevelLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("SecondLevelLoaded"));

	if (second_actor_lazy.IsValid())
	{
		second_actor_lazy->SetActorHiddenInGame(true);
		second_actor_lazy->SetActorEnableCollision(false);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("second_actor_lazy is invalid"));

	//if (first_actor_lazy.IsValid())
	//{
	//	first_actor_lazy->SetActorHiddenInGame(false);
	//	first_actor_lazy->SetActorEnableCollision(true);
	//}
	//else
	//	UE_LOG(LogTemp, Error, TEXT("first_actor_lazy is invalid"));
}