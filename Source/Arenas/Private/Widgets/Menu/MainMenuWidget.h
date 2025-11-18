// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

class UWaitWidget;
class AMainMenuPlayerController;
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
	/* Main */
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY()
	UArenasGameInstance* OwnerArenasGameInstance;
	
	UPROPERTY()
	AMainMenuPlayerController* OwnerMainMenuPlayerController;
	
	void SwitchToMainWidget();
	
	UPROPERTY(meta=(BindWidget))
	UWidget* MainWidgetRoot;
		
	/* Login */
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;
	
	UPROPERTY(meta=(BindWidget))
	UArenasButton* LoginButton;
	
	void SwitchToLoginWidget();
	
	UFUNCTION()
	void OnLoginButtonClicked();
	
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage);
	
	/* Wait */
	UPROPERTY(meta=(BindWidget))
	UWaitWidget* WaitWidget;
	
	FOnButtonClickedEvent& SwitchToWaitWidget(const FText& InWaitInfoText, bool bAllowCancel = false);
	
	/* Camera */
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraBlendTime = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraBlendExp = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FName DefaultCameraTag = FName("Default");
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FName MainMenuCameraTag = FName("MainMenu");
	
	// 切换摄像机视角到指定标签的摄像机，并带有混合效果
	void SwitchCameraByTagWithBlend(const FName& InCameraTag);
	
};
