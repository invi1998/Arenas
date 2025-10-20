// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchFinished.generated.h"

class UArenasButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARENAS_API UMatchFinished : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void SetMenuTitleText(const FText& NewTitleText);

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnMatchFinishedDisplayed(bool bIsWin);

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadWrite)
	UTextBlock* MenuTitle;

private:

	UPROPERTY(meta=(BindWidget))
	UArenasButton* MainMenuButton;	// 返回主菜单按钮

	UPROPERTY(meta=(BindWidget))
	UArenasButton* QuitGameButton;	// 退出游戏按钮

	UFUNCTION()
	void OnMainMenuButtonClicked();

	UFUNCTION()
	void OnQuitGameButtonClicked();
};
