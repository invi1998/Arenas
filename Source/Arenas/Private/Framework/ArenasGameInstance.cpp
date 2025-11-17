// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGameInstance.h"

#include "Network/ArenasNetFunctionLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UArenasGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
	{
		// 如果是在专用服务器上运行，或者是监听服务器，则加载关卡并监听
		LoadLevelAndListen(GameLevelMap);
	}
}

void UArenasGameInstance::Init()
{
	Super::Init();
	
	// 启动客户端？服务端？还是连带编辑器也需要启动？
	if (GetWorld()->IsEditorWorld()) return;
	
	if (UArenasNetFunctionLibrary::IsSessionServer(this))
	{
		// 作为服务器启动，则创建会话
		CreateSession();
	}
	
}

void UArenasGameInstance::PlayerJoinedSession(const FUniqueNetIdRepl& UniqueId)
{
	PlayerJoinedSessions.Add(UniqueId);
	if (WaitPlayerJoinSessionTimeoutHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinSessionTimeoutHandle);
	}
}

void UArenasGameInstance::PlayerLeftSession(const FUniqueNetIdRepl& UniqueId)
{
	PlayerJoinedSessions.Remove(UniqueId);
	if (PlayerJoinedSessions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### All players have left the session."));
		TerminateSessionServer();		// 终止会话服务
	}
}

void UArenasGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Session Created Successfully: %s"), *SessionName.ToString());
		// 会话创建成功后，设立一个定时器，等待一段时间后，如果没有玩家加入，则终止会话服务
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinSessionTimeoutHandle);
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinSessionTimeoutHandle, this, &UArenasGameInstance::WaitPlayerJoinSessionTimeout, WaitPlayerJoinTimeoutDuration, false);
		// 会话创建成功后，加载关卡并监听
		LoadLevelAndListen(LobbyMap);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to create session: %s"), *SessionName.ToString());
		TerminateSessionServer();		// 终止会话服务
	}
	
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		// 在会话创建完成后，移除委托
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
	
}

void UArenasGameInstance::CreateSession()
{
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		// 因为我们需要为多场游戏启动多个会话，所以需要一种方式在启动会话的时候告诉他们自己的名称以及应该使用哪个端口
		// 那样在玩家加入或者搜索的时候就能有一个唯一ID使用
		ServerSessionName = UArenasNetFunctionLibrary::GetSessionNameStr();
		FString SessionSearchId = UArenasNetFunctionLibrary::GetSessionSearchIdStr();
		SessionServerPort = UArenasNetFunctionLibrary::GetSessionPort();
		
		FOnlineSessionSettings OnlineSessionSettings = UArenasNetFunctionLibrary::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionServerPort, SessionSearchId);
		
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UArenasGameInstance::OnCreateSessionComplete);
		
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Error, TEXT("#### Failed to create session: %s"), *ServerSessionName);
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();		// 终止会话服务
			return;
		}
	
		UE_LOG(LogTemp, Warning, TEXT("#### Creating Session: %s on Port: %d with SearchId: %s"), *ServerSessionName, SessionServerPort, *SessionSearchId);
	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get SessionPtr when creating session."));
		TerminateSessionServer();		// 终止会话服务
	}
	
}

void UArenasGameInstance::WaitPlayerJoinSessionTimeout()
{
	UE_LOG(LogTemp, Error, TEXT("#### Wait Player Join Session Timeout reached."));
	TerminateSessionServer();		// 终止会话服务
}

void UArenasGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);		// 非强制退出
}

void UArenasGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UArenasGameInstance::OnEndSessionComplete);
		
		if (!SessionPtr->EndSession(FName(ServerSessionName)))
		{
			// 如果结束会话失败，则直接移除委托，并且退出
			UE_LOG(LogTemp, Error, TEXT("#### Failed to end session: %s"), *ServerSessionName);
			SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
			FGenericPlatformMisc::RequestExit(false);		// 非强制退出
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);		// 非强制退出
	}
}

void UArenasGameInstance::LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad)
{
	// 获取关卡包名
	const FName LevelURL = FName(FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));

	if (!LevelURL.IsNone())
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), SessionServerPort);
		UE_LOG(LogTemp, Warning, TEXT("#### Server traveling to level: %s"), *TravelStr);
		GetWorld()->ServerTravel(TravelStr);	// 加载关卡并监听
	}
	
}
