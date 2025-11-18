// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MainMenuWidget.h"

#include "CineCameraActor.h"
#include "Widgets/Component/ArenasButton.h"
#include "Framework/ArenasGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "player/MainMenuPlayerController.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwnerArenasGameInstance = GetGameInstance<UArenasGameInstance>();
	if (OwnerArenasGameInstance)
	{
		OwnerArenasGameInstance->OnLoginCompletedDelegate.AddUObject(this, &UMainMenuWidget::LoginCompleted);
	}
	
	LoginButton->ButtonArea->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	
	OwnerMainMenuPlayerController = Cast<AMainMenuPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	
}

void UMainMenuWidget::OnLoginButtonClicked()
{
	if (OwnerArenasGameInstance)
	{
		OwnerArenasGameInstance->ClientAccountPortalLogin();
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Login Successful. Nickname: %s"), *PlayerNickname);
		SwitchCameraByTagWithBlend(MainMenuCameraTag);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Login Failed. Error: %s"), *ErrorMessage);
	}
}

void UMainMenuWidget::SwitchCameraByTagWithBlend(const FName& InCameraTag)
{
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(this, ACineCameraActor::StaticClass(), InCameraTag, FoundCameras);
	if (!FoundCameras.IsEmpty())
	{
		if (OwnerMainMenuPlayerController)
		{
			OwnerMainMenuPlayerController->SetViewTargetWithBlend(FoundCameras[0], CameraBlendTime, EViewTargetBlendFunction::VTBlend_Cubic, CameraBlendExp);
		}
	}
}

