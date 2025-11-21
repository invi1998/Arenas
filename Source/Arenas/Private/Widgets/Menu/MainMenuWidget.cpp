// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "MainMenuWidget.h"

#include "CineCameraActor.h"
#include "WaitWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Widgets/Component/ArenasButton.h"
#include "SessionEntryWidget.h"
#include "Framework/ArenasGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ArenasNetFunctionLibrary.h"
#include "player/MainMenuPlayerController.h"

#define LOCTEXT_NAMESPACE "MainMenuWidget"
FText LoggingInText = LOCTEXT("LoggingInText", "正在登录");
FText CreatingSessionText = LOCTEXT("CreatingSessionText", "正在创建会话");
FText JoiningSessionText = LOCTEXT("JoiningSessionText", "正在加入会话");
#undef LOCTEXT_NAMESPACE

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwnerMainMenuPlayerController = Cast<AMainMenuPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	
	OwnerArenasGameInstance = GetGameInstance<UArenasGameInstance>();
	if (OwnerArenasGameInstance)
	{
		OwnerArenasGameInstance->OnLoginCompletedDelegate.AddUObject(this, &UMainMenuWidget::LoginCompleted);
		if (OwnerArenasGameInstance->IsLoggedIn())
		{
			SwitchToMainWidget();
		}
		else
		{
			SwitchToLoginWidget();
		}
		
		OwnerArenasGameInstance->JoinSessionFailedDelegate.AddUObject(this, &UMainMenuWidget::JoinSessionFailed);
		OwnerArenasGameInstance->GlobalSessionSearchCompletedDelegate.AddUObject(this, &UMainMenuWidget::UpdateLobbyList);
		OwnerArenasGameInstance->StartGlobalSessionSearch();
	}
	
	LoginButton->ButtonArea->OnClicked.AddDynamic(this, &UMainMenuWidget::OnLoginButtonClicked);
	
	CreateSessionButton->ButtonArea->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCreateSessionButtonClicked);
	CreateSessionButton->SetIsEnabled(false);
	
	NewSessionNameText->OnTextChanged.AddDynamic(this, &UMainMenuWidget::OnNewSessionNameTextChanged);
	
	JoinSessionButton->SetIsEnabled(false);
	JoinSessionButton->ButtonArea->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinSessionButtonClicked);
}

void UMainMenuWidget::SwitchToMainWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(MainWidgetRoot);
		SwitchCameraByTagWithBlend(MainMenuCameraTag);
	}
}

void UMainMenuWidget::CancelCreateSession()
{
	if (OwnerArenasGameInstance)
	{
		OwnerArenasGameInstance->CancelCreateSession();
	}
	SwitchToMainWidget();
}

void UMainMenuWidget::OnCreateSessionButtonClicked()
{
	if (OwnerArenasGameInstance && OwnerArenasGameInstance->IsLoggedIn())
	{
		OwnerArenasGameInstance->RequestCreateAndJoinSession(FName(NewSessionNameText->GetText().ToString()));
		SwitchToWaitWidget(CreatingSessionText, true, true).AddDynamic(this, &UMainMenuWidget::CancelCreateSession);
	}
}

void UMainMenuWidget::OnNewSessionNameTextChanged(const FText& InText)
{
	if (InText.IsEmpty())
	{
		CreateSessionButton->SetIsEnabled(false);
	}
	else
	{
		CreateSessionButton->SetIsEnabled(true);
	}
}

void UMainMenuWidget::JoinSessionFailed()
{
	SwitchToMainWidget();
}

void UMainMenuWidget::SessionEntrySelected(const FString& InSessionIDStr)
{
	CurrentSelectedSessionIDStr = InSessionIDStr;
	JoinSessionButton->SetIsEnabled(true);
}

void UMainMenuWidget::UpdateLobbyList(const TArray<FOnlineSessionSearchResult>& InOnlineSessionSearchResults)
{
	LobbySessionListScrollBox->ClearChildren();
	bool bCurrentSelectedSessionStillValid = false;
	for (const FOnlineSessionSearchResult& SearchResult : InOnlineSessionSearchResults)
	{
		USessionEntryWidget* NewEntryWidget = CreateWidget<USessionEntryWidget>(GetOwningPlayer(), SessionEntryWidgetClass);
		if (NewEntryWidget)
		{
			FString SessionName = "";
			SearchResult.Session.SessionSettings.Get(UArenasNetFunctionLibrary::GetSessionNameKey(), SessionName);
			FString SessionIDStr = SearchResult.Session.GetSessionIdStr();
			
			NewEntryWidget->InitializeEntry(SessionName, SessionIDStr, SearchResult);
			
			NewEntryWidget->OnSessionEntrySelectedDelegate.AddUObject(this, &UMainMenuWidget::SessionEntrySelected);
			
			LobbySessionListScrollBox->AddChild(NewEntryWidget);
			
			if (CurrentSelectedSessionIDStr == SessionIDStr)
			{
				bCurrentSelectedSessionStillValid = true;
			}
		}
	}
	
	CurrentSelectedSessionIDStr = bCurrentSelectedSessionStillValid ? CurrentSelectedSessionIDStr : "";
	JoinSessionButton->SetIsEnabled(bCurrentSelectedSessionStillValid);
}

void UMainMenuWidget::OnJoinSessionButtonClicked()
{
	// 尝试加入所选会话
	if (!CurrentSelectedSessionIDStr.IsEmpty() && OwnerArenasGameInstance && OwnerArenasGameInstance->IsLoggedIn())
	{
		if (OwnerArenasGameInstance->JoinSessionWithId(CurrentSelectedSessionIDStr))
		{
			SwitchToWaitWidget(JoiningSessionText, false, true);
		}
	}
}

void UMainMenuWidget::SwitchToLoginWidget()
{
	if (MainSwitcher)
	{
		MainSwitcher->SetActiveWidget(LoginWidgetRoot);
		SwitchCameraByTagWithBlend(DefaultCameraTag);
	}
}

void UMainMenuWidget::OnLoginButtonClicked()
{
	if (OwnerArenasGameInstance && !OwnerArenasGameInstance->IsLoggingIn() && !OwnerArenasGameInstance->IsLoggedIn())
	{
		OwnerArenasGameInstance->ClientAccountPortalLogin();
		SwitchToWaitWidget(LoggingInText, false);
	}
}

void UMainMenuWidget::LoginCompleted(bool bWasSuccessful, const FString& PlayerNickname, const FString& ErrorMessage)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Login Successful. Nickname: %s"), *PlayerNickname);
		SwitchToMainWidget();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Login Failed. Error: %s"), *ErrorMessage);
		SwitchToLoginWidget();
	}
}

FOnButtonClickedEvent& UMainMenuWidget::SwitchToWaitWidget(const FText& InWaitInfoText, bool bAllowCancel, bool bSwitchCamera)
{
	MainSwitcher->SetActiveWidget(WaitWidget);
	WaitWidget->SetWaitInfo(InWaitInfoText, bAllowCancel);
	
	if (bSwitchCamera)
	{
		SwitchCameraByTagWithBlend(WaitCameraTag);
	}
	
	return WaitWidget->ClearAndGetButtonClickedEvent();
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

