// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/PlayerInfoTypes.h"
#include "PlayerTeamLayoutWidget.generated.h"

class UHorizontalBox;
class UPlayerTeamSlotWidget;
/**
 * 
 */
UCLASS()
class ARENAS_API UPlayerTeamLayoutWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void UpdatePlayerelection(const TArray<FPlayerSelection>& InPlayerSelections);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	float PlayerTeamSlotWidgetMargin = 15.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	TSubclassOf<UPlayerTeamSlotWidget> PlayerTeamSlotWidgetClass;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* PlayerTeamLeftLayoutBox;

	UPROPERTY(meta=(BindWidget))
	UHorizontalBox* PlayerTeamRightLayoutBox;

	UPROPERTY()
	TArray<UPlayerTeamSlotWidget*> TeamSlotWidgets;
	
};
