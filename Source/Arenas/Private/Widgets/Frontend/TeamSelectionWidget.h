// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectionWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotClickedDelegate, uint8 /*SlotID*/);

/**
 * 
 */
UCLASS()
class ARENAS_API UTeamSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotID(uint8 InSlotID);
	void UpdateSlotInfoText(const FString& PlayerNickName);
	virtual void NativeConstruct() override;
	
	FOnSlotClickedDelegate OnSlotClicked;

private:
	UPROPERTY(meta=(BindWidget))
	UButton* SelectButton;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* InfoTextBlock;

	UFUNCTION()
	void OnSelectButtonClicked();

	uint8 SlotID;
	
};
