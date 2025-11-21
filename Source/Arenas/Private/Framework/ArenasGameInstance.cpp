// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGameInstance.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
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

bool UArenasGameInstance::IsLoggedIn() const
{
	if (IOnlineIdentityPtr IdentityPtr = UArenasNetFunctionLibrary::GetIdentityPtr())
	{
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	}
	return false;
}

bool UArenasGameInstance::IsLoggingIn() const
{
	// 正在登录当且仅当登录委托句柄有效，表示登录过程尚未完成，否则表示未在登录中
	return LoggingInDelegateHandle.IsValid();
}

void UArenasGameInstance::ClientAccountPortalLogin()
{
	// 使用账号门户登录，即不需要提供具体的账号和密码，通过网页进行登录
	ClientLogin("AccountPortal", "", "");
}

void UArenasGameInstance::ClientLoginComplete(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserUniqueNetId, const FString& ErrorMessage)
{
	if (IOnlineIdentityPtr IdentityPtr = UArenasNetFunctionLibrary::GetIdentityPtr())
	{
		// 移除登录完成委托
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);
			LoggingInDelegateHandle.Reset();		// 重置登录委托句柄，表示登录过程已完成
		}
		
		FString PlayerNickName = "";
		if (bWasSuccessful)
		{
			PlayerNickName = IdentityPtr->GetPlayerNickname(UserUniqueNetId);
			UE_LOG(LogTemp, Warning, TEXT("#### Id(%s) Login Successful. Player Nickname: %s"), *UserUniqueNetId.ToString(), *PlayerNickName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("#### Id(%s) Login Failed. Error: %s"), *UserUniqueNetId.ToString(), *ErrorMessage);
		}
		
		OnLoginCompletedDelegate.Broadcast(bWasSuccessful, PlayerNickName, ErrorMessage);
		
	}
	else
	{
		OnLoginCompletedDelegate.Broadcast(bWasSuccessful, "", "无法获取在线身份接口指针...");
	}
}

void UArenasGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UArenasNetFunctionLibrary::GetIdentityPtr())
	{
		if (LoggingInDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);		// 移除之前的登录委托（如果有的话）
			LoggingInDelegateHandle.Reset();
		}
		// 添加登录完成委托，并保存句柄，以便用于检测登录中状态
		LoggingInDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UArenasGameInstance::ClientLoginComplete);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Error, TEXT("#### Id(%s) Failed to initiate login process."), *Id);
			if (LoggingInDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingInDelegateHandle);		// 移除之前的登录委托（如果有的话）
				LoggingInDelegateHandle.Reset();
			}
		}
	}
}

void UArenasGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("#### Request to create and join session: %s"), *NewSessionName.ToString());
	// CreateSession();
	// 在我们的游戏里，创建会话实际就是启动一个服务器实例
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	
	FString CoordinatorURL = UArenasNetFunctionLibrary::GetCoordinatorURL();
	UE_LOG(LogTemp, Warning, TEXT("#### Coordinator URL: %s"), *CoordinatorURL);
	
	FString URL = FString::Printf(TEXT("%s/Session"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("#### Full Coordinator Session URL: %s"), *URL);
	
	Request->SetURL(URL);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	
	FGuid SessionSearchId = FGuid::NewGuid();		// 生成唯一的会话搜索ID
	
	// 构建请求的JSON体
	TSharedPtr<FJsonObject> RequestJsonObject = MakeShareable(new FJsonObject);
	RequestJsonObject->SetStringField(UArenasNetFunctionLibrary::GetSessionNameKey().ToString(), NewSessionName.ToString());
	RequestJsonObject->SetStringField(UArenasNetFunctionLibrary::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());
	
	// 将JSON体序列化为字符串并设置为请求内容
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestJsonObject.ToSharedRef(), Writer);
	Request->SetContentAsString(RequestBody);
	
	Request->OnProcessRequestComplete().BindUObject(this, &UArenasGameInstance::OnCreateAndJoinSessionResponseReceived, NewSessionName, SessionSearchId);
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to send create session request to coordinator."));
	}
}

void UArenasGameInstance::CancelCreateSession()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Cancel create session request received."));
	StopAllSessionFindings();
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
	
	StartGlobalSessionSearch();
	
}

void UArenasGameInstance::StartGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Starting global session search."));
	
	GetWorld()->GetTimerManager().SetTimer(GlobalSessionSearchTimerHandle, 
		FTimerDelegate::CreateUObject(this, &UArenasGameInstance::PerformGlobalSessionSearch),
		GlobalSessionSearchInterval,
		true, 0.0f);
	
}

void UArenasGameInstance::OnCreateAndJoinSessionResponseReceived(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bConnectedSuccessful, FName SessionName, FGuid SessionSearchId)
{
	if (bConnectedSuccessful)
	{
		int32 ResponseCode = HttpResponse->GetResponseCode();
		if (ResponseCode != 200)
		{
			UE_LOG(LogTemp, Error, TEXT("#### Session created response failed with code: %d"), ResponseCode);
			return;
		}
		
		// 解析响应的JSON体
		TSharedPtr<FJsonObject> ResponseJsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (!FJsonSerializer::Deserialize(Reader, ResponseJsonObject) || !ResponseJsonObject.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("#### Failed to parse session created response JSON，please check coordinator server."));
			return;
		}
		
		FString AssignedSessionName = ResponseJsonObject->GetStringField(UArenasNetFunctionLibrary::GetSessionNameKey().ToString());
		FString AssignedSessionSearchId = ResponseJsonObject->GetStringField(UArenasNetFunctionLibrary::GetSessionSearchIdKey().ToString());
		
		// 传递字符串引用，避免不必要的复制
		int AssignedPort = ResponseJsonObject->GetIntegerField(*(UArenasNetFunctionLibrary::GetSessionPortKey().ToString()));
		
		UE_LOG(LogTemp, Warning, TEXT("#### Session created response received: Name=%s, SearchId=%s, Port=%d"), *AssignedSessionName, *AssignedSessionSearchId, AssignedPort);
		
		StartFindCreatedSession(SessionSearchId);
	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Session created response failed to connect to coordinator."));
	}
}

void UArenasGameInstance::StartFindCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("#### Invalid SessionSearchId provided for finding created session."));
		return;
	}
	
	StopAllSessionFindings();
	UE_LOG(LogTemp, Warning, TEXT("#### Starting to find created session with SearchId: %s"), *(SessionSearchId.ToString()));
	
	// 设置定时器，定期调用FindCreatedSession函数，每隔1s查找一次会话
	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimerHandle, 
		FTimerDelegate::CreateUObject(this, &UArenasGameInstance::FindCreatedSession, SessionSearchId),
		FindCreatedSessionInterval,
		true, 0.0f);
	
	// 设置超时定时器，超过指定时间后调用FindCreatedSessionTimeout函数
	GetWorld()->GetTimerManager().SetTimer(FindCreatedSessionTimeoutTimerHandle,
		this,
		&UArenasGameInstance::FindCreatedSessionTimeout,
		FindCreatedSessionTimeoutDuration,
		false);
	
}

void UArenasGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Stopping all session findings."));
	StopFindingCreatedSession();
	StopGlobalSessionFindings();
}

void UArenasGameInstance::StopFindingCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Stopping finding created session."));
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimeoutTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FindCreatedSessionTimerHandle);
	
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	}
}

void UArenasGameInstance::StopGlobalSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Stopping global session findings."));
	if (GlobalSessionSearchTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(GlobalSessionSearchTimerHandle);
	}
	
	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UArenasGameInstance::OnGlobalSessionSearchCompleted(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (CurrentSessionSearch.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("#### Global session search found %d sessions."), CurrentSessionSearch->SearchResults.Num());
			// 这里可以处理搜索结果，例如显示在UI上供玩家选择加入
			GlobalSessionSearchCompletedDelegate.Broadcast(CurrentSessionSearch->SearchResults);
			
			for (const FOnlineSessionSearchResult& SearchResult : CurrentSessionSearch->SearchResults)
			{
				FString FoundSessionName = "";
				SearchResult.Session.SessionSettings.Get(UArenasNetFunctionLibrary::GetSessionNameKey(), FoundSessionName);
				UE_LOG(LogTemp, Warning, TEXT("#### Found Session: %s"), *FoundSessionName);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Global session search Completed with failure."));
	}

	if (IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr())
	{
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
	
	
}

void UArenasGameInstance::PerformGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("####  -------- Performing global session search. ---------"));
	IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get session ptr."));
		return;
	}
	CurrentSessionSearch = MakeShareable(new FOnlineSessionSearch());
	CurrentSessionSearch->MaxSearchResults = 20;
	CurrentSessionSearch->bIsLanQuery = false;		// 非局域网查询
	
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UArenasGameInstance::OnGlobalSessionSearchCompleted);
	if (!SessionPtr->FindSessions(0, CurrentSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to find sessions."));
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
	
}

void UArenasGameInstance::FindCreatedSessionComplete(bool bWasSuccessful)
{
	if (!bWasSuccessful || !CurrentSessionSearch.IsValid() || CurrentSessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### No created session found yet."));
		return;
	}
	
	StopFindingCreatedSession();
	
	JoinSessionWithSearchResult(CurrentSessionSearch->SearchResults[0]);
	
}

void UArenasGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("#### Finding created session."));
	
	IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get session ptr."));
		return;
	}
	
	CurrentSessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (!CurrentSessionSearch)
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get session ptr."));
		return;
	}
	
	// 设置搜索参数
	CurrentSessionSearch->MaxSearchResults = 1;
	CurrentSessionSearch->bIsLanQuery = false;		// 非局域网查询
	// 根据会话搜索ID进行过滤
	CurrentSessionSearch->QuerySettings.Set(UArenasNetFunctionLibrary::GetSessionSearchIdKey(), SessionSearchId.ToString(), EOnlineComparisonOp::Equals);
	
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UArenasGameInstance::FindCreatedSessionComplete);
	if (!SessionPtr->FindSessions(0, CurrentSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to initiate find sessions."));
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
		return;
	}
	
}

void UArenasGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("#### Finding created session timeout."));
}

void UArenasGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult, int Port)
{
	IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get session ptr."));
		JoinSessionFailedDelegate.Broadcast();
		return;
	}
	
	if (JoinSessionResult == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("#### Joined session successfully: %s"), *SessionName.ToString());
		StopAllSessionFindings();
		
		FString TravelURL = "";
		SessionPtr->GetResolvedConnectString(SessionName, TravelURL);
		
		FString TestingURL = UArenasNetFunctionLibrary::GetTestingURL();
		if (!TestingURL.IsEmpty())
		{
			TravelURL = TestingURL;
		}
		
		UArenasNetFunctionLibrary::ReplacePortInURL(TravelURL, Port);
		UE_LOG(LogTemp, Warning, TEXT("#### Traveling to session at URL: %s"), *TravelURL);
		
		GetFirstLocalPlayerController(GetWorld())->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to join session: %s with result: %d"), *SessionName.ToString(), static_cast<int>(JoinSessionResult));
		JoinSessionFailedDelegate.Broadcast();
	}
	
	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	
}

void UArenasGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("#### Joining session with search result."));
	IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to get session ptr."));
		return;
	}
	
	// 获取会话名称
	FString SessionName = "";
	SearchResult.Session.SessionSettings.Get(UArenasNetFunctionLibrary::GetSessionNameKey(), SessionName);
	// 获取会话端口
	int64 Port = 7777;		// 默认端口
	if (const FOnlineSessionSetting* PortSetting = SearchResult.Session.SessionSettings.Settings.Find(UArenasNetFunctionLibrary::GetSessionPortKey()))
	{
		 PortSetting->Data.GetValue(Port);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("#### Joining Session: %s on Port: %lld"), *SessionName, Port);
	
	SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
	SessionPtr->OnJoinSessionCompleteDelegates.AddUObject(this, &UArenasGameInstance::OnJoinSessionComplete, static_cast<int>(Port));
	
	if (!SessionPtr->JoinSession(0, FName(*SessionName), SearchResult))
	{
		UE_LOG(LogTemp, Error, TEXT("#### Failed to initiate join session: %s"), *SessionName);
		SessionPtr->OnJoinSessionCompleteDelegates.RemoveAll(this);
		JoinSessionFailedDelegate.Broadcast();
		return;
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
