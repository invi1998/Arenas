// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "SessionEntryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Network/ArenasNetFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "USessionEntryWidget"
FText UnKnownPlayerCountText = NSLOCTEXT("USessionEntryWidget", "UnKnownPlayerCount", "未知玩家数量");
#undef LOCTEXT_NAMESPACE

void USessionEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SessionButton->OnClicked.AddDynamic(this, &USessionEntryWidget::OnSessionButtonClicked);
}

void USessionEntryWidget::NativeDestruct()
{
	// 清理委托
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->ClearOnUpdateSessionCompleteDelegates(this);
		SessionPtr->ClearOnSessionParticipantJoinedDelegates(this);
		SessionPtr->ClearOnSessionParticipantLeftDelegates(this);
	}
	
	Super::NativeDestruct();
}

void USessionEntryWidget::InitializeEntry(const FString& InSessionName, const FString& InSessionIDString, const FOnlineSessionSearchResult& InSessionSearchResult)
{
	SessionNameText->SetText(FText::FromString(InSessionName));
	CachedSessionIDString = InSessionIDString;
	CachedSessionSearchResult = InSessionSearchResult;
	
	UpdatePlayerCountDisplay();
	
	// 监听会话里的玩家数量，以及最大玩家数量
	
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		// 监听会话更新完成事件
		SessionPtr->AddOnUpdateSessionCompleteDelegate_Handle(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &USessionEntryWidget::OnSessionUpdated));
		// 监听玩家加入/离开会话事件
		SessionPtr->AddOnSessionParticipantJoinedDelegate_Handle(FOnSessionParticipantJoinedDelegate::CreateUObject(this, &USessionEntryWidget::OnSessionParticipantJoin));
		SessionPtr->AddOnSessionParticipantLeftDelegate_Handle(FOnSessionParticipantLeftDelegate::CreateUObject(this, &USessionEntryWidget::OnSessionParticipantLeft));
	}

	
}

void USessionEntryWidget::OnSessionButtonClicked()
{
	OnSessionEntrySelectedDelegate.Broadcast(CachedSessionIDString);
}

void USessionEntryWidget::OnSessionUpdated(FName SessionName, bool bWasSuccessful)
{
	UpdatePlayerCountDisplay();
}

void USessionEntryWidget::OnSessionParticipantJoin(FName SessionName, const FUniqueNetId& NetId)
{
	UpdatePlayerCountDisplay();
}

void USessionEntryWidget::OnSessionParticipantLeft(FName SessionName, const FUniqueNetId& NetId, EOnSessionParticipantLeftReason OnSessionParticipantLeftReason)
{
	UpdatePlayerCountDisplay();
}

void USessionEntryWidget::UpdatePlayerCountDisplay()
{
	if (CachedSessionSearchResult.IsValid())
	{
		// 更安全的计算方式
		int32 MaxPlayers = CachedSessionSearchResult.Session.SessionSettings.NumPublicConnections;
		// return Result.OnlineResult.Session.SessionSettings.NumPublicConnections - Result.OnlineResult.Session.NumOpenPublicConnections;
		int32 CurrentPlayers = FMath::Max(0, MaxPlayers - CachedSessionSearchResult.Session.NumOpenPublicConnections);

		UE_LOG(LogTemp, Warning, TEXT("#### Session %s Player Count: %d/%d"), *CachedSessionIDString, CurrentPlayers, MaxPlayers);
		
		// 添加边界检查
		CurrentPlayers = FMath::Clamp(CurrentPlayers, 0, MaxPlayers);
		PlayerAmount->SetText(FText::Format(NSLOCTEXT("Game", "PlayerCount", "{0}/{1} 玩家"), CurrentPlayers, MaxPlayers));
	}
	else
	{
		PlayerAmount->SetText(UnKnownPlayerCountText);
	}
	
}
