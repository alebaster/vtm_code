#include "RuntimeUtils.h"

#include "Runtime/Engine/Public/EngineUtils.h"

#include "Runtime/Core/Public/Modules/ModuleManager.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"

#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimMontage.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"

UTexture2D* URuntimeUtils::LoadTexture2D_FromFile(const FString& FullFilePath, EImageFormat ImageFormat, bool& IsValid, int32& Width, int32& Height)
{
	UTexture2D* ret = 0;
	IsValid = false;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	//Please use 'TSharedPtr<IImageWrapper>'
	IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
		return NULL;

	//Create T2D
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		const TArray<uint8>* UncompressedBGRA = NULL;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			ret = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

			if (!ret)
				return NULL;

			Width = ImageWrapper->GetWidth();
			Height = ImageWrapper->GetHeight();

			void* TextureData = ret->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedBGRA->GetData(), UncompressedBGRA->Num());
			ret->PlatformData->Mips[0].BulkData.Unlock();

			ret->UpdateResource();
		}
	}

	return ret;
}

USoundWave* URuntimeUtils::GetSoundWave_FromFile(const FString& FullFilePath)
{
	USoundWave* ret = 0;

	//Load From File
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *FullFilePath))
		return NULL;

	ret = NewObject<USoundWave>(USoundWave::StaticClass());

	FWaveModInfo WaveInfo;
	if (WaveInfo.ReadWaveInfo(RawFileData.GetData(), RawFileData.Num()))
	{
		ret->InvalidateCompressedData();

		ret->RawData.Lock(LOCK_READ_WRITE);
		void* LockedData = ret->RawData.Realloc(RawFileData.Num());
		FMemory::Memcpy(LockedData, RawFileData.GetData(), RawFileData.Num());
		ret->RawData.Unlock();

		int32 DurationDiv = *WaveInfo.pChannels * *WaveInfo.pBitsPerSample * *WaveInfo.pSamplesPerSec;
		if (DurationDiv)
			ret->Duration = *WaveInfo.pWaveDataSize * 8.0f / DurationDiv;
		else
			ret->Duration = 0.0f;
		ret->SetSampleRate(*WaveInfo.pSamplesPerSec);
		ret->NumChannels = *WaveInfo.pChannels;
		ret->RawPCMDataSize = WaveInfo.SampleDataSize;
		ret->SoundGroup = ESoundGroup::SOUNDGROUP_Default;

		// standalone fix
		//ret->RawPCMDataSize = WaveInfo.SampleDataSize;
		//ret->RawPCMData = (uint8*)FMemory::Malloc(ret->RawPCMDataSize);
		//FMemory::Memmove(ret->RawPCMData, RawFileData.GetData(), RawFileData.Num());
	}
	else
		ret = 0;

	return ret;
}

float URuntimeUtils::PlayMontage(USkeletalMeshComponent* me, UAnimationAsset* a, bool loop)
{
	if (!me || !a)
		return 0.0f;

	float ret = 0.0f;

	me->PlayAnimation(a, loop);

	UAnimSequenceBase* seq = static_cast<UAnimSequenceBase*>(a);
	UAnimSingleNodeInstance* SingleNodeInstance = me->GetSingleNodeInstance();
	if (SingleNodeInstance)
		ret = SingleNodeInstance->GetLength()/seq->RateScale;

	return ret;
}

void URuntimeUtils::StopMontage(USkeletalMeshComponent* me, UAnimationAsset* a)
{
	if (!me || !a)
		return;

	me->Stop();
}

float URuntimeUtils::GetAnimSequenceLength(UAnimSequence* AnimSequence)
{
	float ret=0.0f;

	if (AnimSequence)
	{
		if (AnimSequence->RateScale != 0)
			ret = AnimSequence->SequenceLength / AnimSequence->RateScale;
	}

	return ret;
}