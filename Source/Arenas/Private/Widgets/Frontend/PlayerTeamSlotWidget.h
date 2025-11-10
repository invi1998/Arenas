// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ARENAS_API UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	UImage* PlayerCharacterImage;		// 玩家角色图像

	UPROPERTY(meta=(BindWidget))
	UTextBlock* NameTextBlock;
	
	
};
