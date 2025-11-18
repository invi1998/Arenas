// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MainMenuWidget.h"

#include "Widgets/Component/ArenasButton.h"
#include "Framework/ArenasGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwnerArenasGameInstance = GetGameInstance<UArenasGameInstance>();
	if (OwnerArenasGameInstance)
	{
		
	}
	
	LoginButton->ButtonArea->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	
}

void UMainMenuWidget::OnLoginButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Login Button Clicked"));
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Login Successful. Nickname: %s"), *PlayerNickname);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Login Failed. Error: %s"), *ErrorMessage);
	}
}
