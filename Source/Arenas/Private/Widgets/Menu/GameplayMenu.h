// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameplayMenu.generated.h"

class UArenasButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ARENAS_API UGameplayMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	FOnButtonClickedEvent& GetResumeButtonClickedEventDelegate() const;

private:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MenuTitle;

	UPROPERTY(meta=(BindWidget))
	UArenasButton* ResumeButton;	// 继续游戏按钮

	UPROPERTY(meta=(BindWidget))
	UArenasButton* MainMenuButton;	// 返回主菜单按钮

	UPROPERTY(meta=(BindWidget))
	UArenasButton* QuitGameButton;	// 退出游戏按钮

	UFUNCTION()
	void OnMainMenuButtonClicked();

	UFUNCTION()
	void OnQuitGameButtonClicked();
	
};
