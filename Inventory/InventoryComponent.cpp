#include "InventoryComponent.h"

#include "Core/RuntimeUtils.h"

#include "Character/MainCharacter.h"
#include "VtmPlayerController.h"
#include "UI/MainHUD.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Error, TEXT("hey"));

	FString path = FPaths::ProjectContentDir() + "RuntimeResources/item/item_db.json";
	FString json_string;
	if (FFileHelper::LoadFileToString(json_string, *path))
		ParseJson(json_string);
	else
		UE_LOG(LogTemp, Error, TEXT("cannot open item_db file"));

	UE_LOG(LogTemp, Warning, TEXT("item_db size: %d"), items_db.Num());
}

FInventoryItemBase* UInventoryComponent::AddItem(FString id, int amount)
{
	FInventoryItemBase* ret=0;

	FInventoryItemBase** item_ptr = items_db.Find(id);
	if (!item_ptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("cannot find in db item id: %s"), *id);
		return ret;
	}
	
    // new or ++?
    FInventoryItemBase** item_inv = inventory.Find(id);
	if (!item_inv) // new
    {
        FInventoryItemBase* new_item = (*item_ptr)->clone();
        inventory.Add(id, new_item);
        new_item->count += amount;
		ret = new_item;
    }
	else // ++
	{
		(*item_inv)->count += amount;
		ret = *item_inv;
	}

	// show notification
	AMainCharacter* owner = 0;
	AVtmPlayerController* controller = Cast<AVtmPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (controller)
		owner = Cast<AMainCharacter>(controller->GetCharacter());
	if (owner)
	{
		UMainHUD* hud = owner->GetHud();
		if (hud)
			hud->ShowItemTake((*item_ptr)->name);
		else
			UE_LOG(LogTemp, Error, TEXT("hud in inventory component is null"));
	}
	else
		UE_LOG(LogTemp, Error, TEXT("controller in inventory component is null"));

	UE_LOG(LogTemp, Warning, TEXT("added item: %s"), *id);

	return ret;
}

bool UInventoryComponent::HasItem(FString id)
{
	return inventory.Contains(id);
}

void UInventoryComponent::GiveMoney(int amount)
{

}

FInventoryItemBase* UInventoryComponent::GetItem(FString id)
{
	FInventoryItemBase* ret = 0;

	FInventoryItemBase** item_ptr = inventory.Find(id);
	if (item_ptr)
		ret = *item_ptr;

	return ret;
}

FInventoryItemBase* UInventoryComponent::GetItemFromDB(FString id)
{
	FInventoryItemBase* ret = 0;

	FInventoryItemBase** item_ptr = items_db.Find(id);
	if (item_ptr)
		ret = *item_ptr;

	return ret;
}

TArray <FInventoryItemBase*> UInventoryComponent::GetAllItemsOfType(EIIType type)
{
	TArray <FInventoryItemBase*> ret;

	for (auto item_pair : inventory)
	{
		if (item_pair.Value->type == type)
			ret.Add(item_pair.Value);
	}

	return ret;
}

// json processing
void UInventoryComponent::ParseJson(FString& string)
{
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(string);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("json item_db file is not valid"));
		return;
	}

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FillStructs(JsonObject);
	}
	else
		UE_LOG(LogTemp, Error, TEXT("cannot decerialize json item_db file"));
}

void UInventoryComponent::FillStructs(TSharedPtr<FJsonObject> jobj)
{
	get_items(jobj);
	get_melee(jobj);
	get_ranged(jobj);
	get_armor(jobj);
}

void UInventoryComponent::get_items(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> json_lines = jobj->GetArrayField(TEXT("Items"));
	for (int32 i = 0; i < json_lines.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_lines[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		//UInventoryItem* item = NewObject<UInventoryItem>(UInventoryItem::StaticClass());
		FInventoryItemBase* item = new FInventoryItemBase();

		item->name = object->GetStringField(TEXT("display_name"));
		item->id = object->GetStringField(TEXT("id"));
		item->type = (EIIType)object->GetIntegerField(TEXT("type"));
		item->icon_name = object->GetStringField(TEXT("icon"));
		item->description = object->GetStringField(TEXT("description"));

		items_db.Add(item->id, item);
	}
}

void UInventoryComponent::get_melee(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> json_lines = jobj->GetArrayField(TEXT("MeleeWeapons"));
	for (int32 i = 0; i < json_lines.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_lines[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FMeleeItem* item = new FMeleeItem();

		item->name = object->GetStringField(TEXT("display_name"));
		item->id = object->GetStringField(TEXT("id"));
		item->type = (EIIType)object->GetIntegerField(TEXT("type"));
		item->icon_name = object->GetStringField(TEXT("icon"));
		item->description = object->GetStringField(TEXT("description"));
		// addons

		items_db.Add(item->id, item);
	}
}

void UInventoryComponent::get_ranged(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> json_lines = jobj->GetArrayField(TEXT("RangedWeapons"));
	for (int32 i = 0; i < json_lines.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_lines[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FRangedItem* item = new FRangedItem();

		item->name = object->GetStringField(TEXT("display_name"));
		item->id = object->GetStringField(TEXT("id"));
		item->type = (EIIType)object->GetIntegerField(TEXT("type"));
		item->icon_name = object->GetStringField(TEXT("icon"));
		item->description = object->GetStringField(TEXT("description"));
		item->range = object->GetIntegerField(TEXT("range"));
		item->clip_size = object->GetIntegerField(TEXT("clip_size"));
		item->spread = object->GetIntegerField(TEXT("spread"));
		// addons

		items_db.Add(item->id, item);
	}
}

void UInventoryComponent::get_armor(TSharedPtr<FJsonObject>& jobj)
{
	TArray<TSharedPtr<FJsonValue>> json_lines = jobj->GetArrayField(TEXT("Armors"));
	for (int32 i = 0; i < json_lines.Num(); ++i)
	{
		TSharedPtr<FJsonValue> value = json_lines[i];
		TSharedPtr<FJsonObject> object = value->AsObject();

		FArmorItem* item = new FArmorItem();

		item->name = object->GetStringField(TEXT("display_name"));
		item->id = object->GetStringField(TEXT("id"));
		item->type = (EIIType)object->GetIntegerField(TEXT("type"));
		item->icon_name = object->GetStringField(TEXT("icon"));
		item->description = object->GetStringField(TEXT("description"));
		// addons

		items_db.Add(item->id, item);
	}
}