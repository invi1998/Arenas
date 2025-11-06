// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UUniformGridPanel;
class UArenasButton;
class UWidgetSwitcher;
/**
 * 大厅界面Widget
 */
UCLASS()
class ARENAS_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;

	UPROPERTY(meta=(BindWidget))
	UWidget* TeamSelectionRoot;

	UPROPERTY(meta=(BindWidget))
	UArenasButton* StartHeroSelectionButton;

	UPROPERTY(meta=(BindWidget))
	UUniformGridPanel* TeamSelectionSlotGridPanel;		// 队伍选择槽的网格面板

	
};
