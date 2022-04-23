#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Core/Public/Containers/Map.h"

#include "Inventory/InventoryItem.h"

#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VTM_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	FInventoryItemBase* AddItem(FString id, int amount=1);
	bool HasItem(FString id);

	//FString GetLastMeleeWeaponId() { return last_melee; }
	//FString GetLastRangedWeaponId() { return last_ranged; }

	void GiveMoney(int amount);

	TMap <FString, FInventoryItemBase*>* GetInventory() { return &inventory; }
	TArray <FInventoryItemBase*> GetAllItemsOfType(EIIType type);
	FInventoryItemBase* GetItem(FString id);
	FInventoryItemBase* GetItemFromDB(FString id);

protected:
	virtual void BeginPlay() override;

private:
	// data
	//UPROPERTY()
	TMap <FString, FInventoryItemBase*> inventory;

	TMap <FString, FInventoryItemBase*> items_db;

	//FString last_melee;
	//FString last_ranged;

	// json processing
	void ParseJson(FString& string);
	void FillStructs(TSharedPtr<FJsonObject> jobj);
	void get_items(TSharedPtr<FJsonObject>& jobj);
	void get_melee(TSharedPtr<FJsonObject>& jobj);
	void get_ranged(TSharedPtr<FJsonObject>& jobj);
	void get_armor(TSharedPtr<FJsonObject>& jobj);
};
