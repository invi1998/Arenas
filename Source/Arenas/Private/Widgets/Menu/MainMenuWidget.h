// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

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
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY()
	UArenasGameInstance* OwnerArenasGameInstance;
	
	UPROPERTY(meta=(BindWidget))
	UWidget* LoginWidgetRoot;
	
	UPROPERTY(meta=(BindWidget))
	UArenasButton* LoginButton;
	
	UPROPERTY()
	AMainMenuPlayerController* OwnerMainMenuPlayerController;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraBlendTime = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	float CameraBlendExp = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FName DefaultCameraTag = FName("Default");
	
	UPROPERTY(EditDefaultsOnly, Category="Camera")
	FName MainMenuCameraTag = FName("MainMenu");
	
	UFUNCTION()
	void OnLoginButtonClicked();
	
	void LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage);
	
	// 切换摄像机视角到指定标签的摄像机，并带有混合效果
	void SwitchCameraByTagWithBlend(const FName& InCameraTag);
	
};
