#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Actors/UsableActor.h"

#include "UsableWidgetComponent.generated.h"

class ULuaComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UUsableWidgetComponent : public UActorComponent, public IUsableActor
{
	GENERATED_BODY()

public:	
	UUsableWidgetComponent();

	UPROPERTY(EditAnywhere)
	FString lua_script;

	UFUNCTION()
	virtual UTexture2D* GetActionImage() override;

	UFUNCTION()
	virtual int Use(AActor* who) override;
    
    UFUNCTION()
	virtual void EndUse(AActor* who) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UTexture2D* ActionImage=0;

	UPROPERTY()
	ULuaComponent* lua=0;

	UPROPERTY(EditAnywhere)
	UUserWidget* widget=0;
};
