#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Actors/UsableActor.h"
#include "Inventory/InventoryItem.h"

#include "LootActorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API ULootActorComponent : public UActorComponent, public IUsableActor
{
	GENERATED_BODY()

public:	
	ULootActorComponent();
    
    UPROPERTY(EditAnywhere)
    TArray<FLiteInvItem> loot;
    
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
    
    void SetAnimOpen();
    void SetAnimClose();
};
