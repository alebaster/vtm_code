#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"

#include "Runtime/Engine/Classes/Sound/SoundWave.h"

#include "RuntimeUtils.generated.h"

class URangedWeapon;
class UAnimSequence;

class USkeletalMeshComponent;
class UAnimationAsset;

UCLASS()
class VTM_API URuntimeUtils : public UObject
{
	GENERATED_BODY()

public:
	//UFUNCTION()
	static UTexture2D* LoadTexture2D_FromFile(const FString& FullFilePath, EImageFormat ImageFormat, bool& IsValid, int32& Width, int32& Height);
    static USoundWave* GetSoundWave_FromFile(const FString& FullFilePath);

    static float PlayMontage(USkeletalMeshComponent* me, UAnimationAsset* a, bool loop=false);
    static void StopMontage(USkeletalMeshComponent* me, UAnimationAsset* a);

	static float GetAnimSequenceLength(UAnimSequence* AnimSequence);

	template <typename T>
	static UClass* GetBlueprint(const FString& path, const FString& name)
	{
		FStringClassReference ref("Blueprint'"+path+name+"."+name+"_C'");
		ref.TryLoadClass<T>();
		UClass* bp_class = 0;
		bp_class = ref.ResolveClass();

		return bp_class;
	}

    // TSubclassOf<T> bp = LoadBlueprint_FromFile<T>("","");
    template <typename T>
    static T* LoadBlueprint_FromFile(const FString& FullFilePath, const FString& FileName)
    {
        FString name = FileName+"_C";
        TArray<UObject*> assets;
        if (EngineUtils::FindOrLoadAssetsByPath(*FullFilePath,assets,EngineUtils::ATL_Class))
        {
            for (int i=0;i<assets.Num();++i)
            {
                UObject* uo = assets[i];
				if (!Cast<T>(uo) || uo->GetName().Compare(name))
					continue;
                
				return Cast<T>(uo);
            }
        }
        
        return 0;
    }
};
