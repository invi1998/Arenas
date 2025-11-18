// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UArenasButton;
class UArenasGameInstance;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class ARENAS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY()
	UArenasGameInstance* OwnerArenasGameInstance;
	
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;
	
	UPROPERTY(meta=(BindWidget))
	UArenasButton* LoginButton;
	
	UFUNCTION()
	void OnLoginButtonClicked();
	
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage);
	
};
