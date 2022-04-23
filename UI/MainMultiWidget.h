#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// umg
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/SlateCore/Public/Styling/SlateColor.h"
#include "Runtime/UMG/Public/Components/WidgetSwitcher.h"
#include "Runtime/UMG/Public/Components/HorizontalBox.h"

#include "Runtime/Engine/Classes/Components/AudioComponent.h"

#include "Runtime/Core/Public/Containers/Array.h"
#include "Runtime/Core/Public/Containers/Map.h"

#include "UI/CharacterWidget.h"
#include "UI/InfoWidget.h"
#include "UI/JournalWidget.h"

#include "MainMultiWidget.generated.h"

USTRUCT()
struct FSheetDescr
{
	GENERATED_BODY()

	FString id;
	FString display_name;
	TArray <FString> links;
	FString description;
};

UCLASS()
class VTM_API UMainMultiWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool OnScreen() { return IsInViewport(); }

	void ShowCharacter();
	void ShowInfo();
	void ShowJournal();

	void Hide();

	void UpdateHoverCharacter(FString key1, FString key2);

	UJournalWidget* GetJournalWidget() { return journal_widget; }

	UPROPERTY(EditAnywhere)
	USoundWave* BackgroundMusic=0;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UAudioComponent* music=0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCharacterWidget> character_widget_bp = 0;

	UPROPERTY()
	UCharacterWidget* character_widget = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInfoWidget> info_widget_bp = 0;

	UPROPERTY()
	UInfoWidget* info_widget = 0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UJournalWidget> journal_widget_bp = 0;

	UPROPERTY()
	UJournalWidget* journal_widget = 0;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UWidgetSwitcher* Switcher;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FSlateColor OriginalButtonColor;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FSlateColor ActiveButtonColor;

	UPROPERTY(meta = (BindWidget))
	UButton* SheetButton = 0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SheetText = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* InfoButton = 0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText = 0;

	UPROPERTY(meta = (BindWidget))
	UButton* QuestButton = 0;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestText = 0;

	void init();
	void PlayMusic();
	void StopMusic();

	UFUNCTION()
	void ClickSheetButton();
	UFUNCTION()
	void ClickInfoButton();
	UFUNCTION()
	void ClickQuestButton();

	// data
	UPROPERTY()
	TMap <FString, FSheetDescr> sheet_data;

	void ParseJson(FString& string);
	void FillSheetStruct(TSharedPtr<FJsonObject> jobj);
	void get_sections(TSharedPtr<FJsonObject>& jobj);
	void get_attributes(TSharedPtr<FJsonObject>& jobj);
	void get_feats(TSharedPtr<FJsonObject>& jobj);
	void get_links(FString name, TArray<FString>& links, TSharedPtr<FJsonObject>& jobj);
};
