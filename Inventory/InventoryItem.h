#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"


#include "InventoryItem.generated.h"

UENUM()
enum EIIType
{
	armor,
	melee,
	ranged,
	key,
	etc
};

USTRUCT()
struct FInventoryItemBase
{
	GENERATED_BODY()

	EIIType type;
	FString id;
	FString name;
	FString description;
	uint8_t weight;
	uint8_t price;
	FString icon_name;
    uint8_t count;
    
    // ???
    //FInventoryItemBase(const FInventoryItemBase& another)
    //{
    //    
    //}
    
    virtual FInventoryItemBase* clone() const
    {
        return new FInventoryItemBase(*this);
    }

	virtual ~FInventoryItemBase() {}
};

USTRUCT()
struct FArmorItem : public FInventoryItemBase
{
	GENERATED_BODY()

	uint8_t rating;
	uint8_t penalty;
    
    virtual FInventoryItemBase* clone() const override
    {
        return new FArmorItem(*this);
    }

	virtual ~FArmorItem() override {}
};

USTRUCT()
struct FMeleeItem : public FInventoryItemBase
{
	GENERATED_BODY()

	uint8_t rate;
    
    virtual FInventoryItemBase* clone() const override
    {
        return new FMeleeItem(*this);
    }

	virtual ~FMeleeItem() override {}
};

USTRUCT()
struct FRangedItem : public FInventoryItemBase
{
	GENERATED_BODY()

	uint8_t rate;
	uint8_t clip_size;
	uint8_t reload_time;
	uint8_t range;
	uint8_t spread;
	uint8_t ammo_type;
    
    virtual FInventoryItemBase* clone() const override
    {
        return new FRangedItem(*this);
    }

	virtual ~FRangedItem() override {}
};

USTRUCT(BlueprintType)
struct FLiteInvItem
{
    GENERATED_BODY()
    
	UPROPERTY(EditAnywhere)
    FString id;

	UPROPERTY(EditAnywhere)
    INT16 amount=1;
};

UCLASS()
class VTM_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:	
	UInventoryItem();

	FInventoryItemBase* item;

protected:

private:

};
