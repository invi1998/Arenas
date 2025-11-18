// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WaitWidget.generated.h"

class UTextBlock;
class UArenasButton;
/**
 * 
 */
UCLASS()
class ARENAS_API UWaitWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	FOnButtonClickedEvent& ClearAndGetButtonClickedEvent();
	
	void SetWaitInfo(const FText& InWaitInfo, bool bAllowedCancel = false);
	
private:
	UPROPERTY(meta=(BindWidget))
	UArenasButton* CancelButton;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WaitInfoText;
	
	
	
};
