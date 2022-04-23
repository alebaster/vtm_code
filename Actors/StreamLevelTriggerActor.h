#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"



#include "StreamLevelTriggerActor.generated.h"

UCLASS()
class VTM_API AStreamLevelTriggerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AStreamLevelTriggerActor();

	void enter();
	void leave();

	UPROPERTY(EditAnywhere)
	FString first_level;

	UPROPERTY(EditAnywhere)
	FString second_level;

	UPROPERTY(EditAnywhere)
	TLazyObjectPtr<AActor> first_actor_lazy;

	UPROPERTY(EditAnywhere)
	TLazyObjectPtr<AActor> second_actor_lazy;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UBoxComponent* first_box=0;

	UPROPERTY()
	class UBoxComponent* second_box=0;

	bool activated = false;

	bool in_first_box = false;
	bool in_second_box = false;

	//virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapBegin_first(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd_first(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnOverlapBegin_second(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd_second(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void FirstLevelLoaded();
	UFUNCTION()
	void SecondLevelLoaded();
};
