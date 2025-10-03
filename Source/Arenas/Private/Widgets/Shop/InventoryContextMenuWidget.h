// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InventoryContextMenuWidget.generated.h"

class UArenasButton;

/**
 * 
 */
UCLASS()
class ARENAS_API UInventoryContextMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClickedEvent& GetSellButtonClickedEvent() const;
	FOnButtonClickedEvent& GetUseButtonClickedEvent() const;
	FOnButtonClickedEvent& GetShowInShopButtonClickedEvent() const;

private:
	UPROPERTY(meta=(BindWidget))
	UArenasButton* UseButton;

	UPROPERTY(meta=(BindWidget))
	UArenasButton* SellButton;

	UPROPERTY(meta=(BindWidget))
	UArenasButton* ShowInShopButton;
	
};
