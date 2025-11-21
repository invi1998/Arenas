// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ArenasGameInstance.generated.h"

class FOnlineSessionSearch;
class IHttpResponse;
class IHttpRequest;
class FOnlineSessionSearchResult;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompletedDelegate, bool /*bWasSuccessful*/, const FString& /*PlayerNickName*/, const FString& /*Error*/);
DECLARE_MULTICAST_DELEGATE(FOnJoinSessionFailedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalSessionSearchCompletedDelegate, const TArray<FOnlineSessionSearchResult>& /*SearchResults*/);

/**
 * 
 */
UCLASS()
class ARENAS_API UArenasGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	void StartMatch();
	virtual void Init() override;
	void PlayerJoinedSession(const FUniqueNetIdRepl& UniqueId);		// 玩家加入会话
	void PlayerLeftSession(const FUniqueNetIdRepl& UniqueId);		// 玩家离开会话
	
public:
	/********************************************************/
	/*					Login								*/
	/********************************************************/
	bool IsLoggedIn() const;		// 是否已登录
	bool IsLoggingIn() const;		// 是否正在登录
	void ClientAccountPortalLogin();		// 客户端账号门户登录
	FOnLoginCompletedDelegate OnLoginCompletedDelegate;		// 登录完成委托
	
private:
	void ClientLoginComplete(int NumOfLocalPlayer, bool  bWasSuccessful, const FUniqueNetId& UserUniqueNetId, const FString& ErrorMessage);
	void ClientLogin(const FString& Type, const FString& Id, const FString& Token);		// 客户端登录
	FDelegateHandle LoggingInDelegateHandle;		// 登录委托句柄
	
public:
	/********************************************************/
	/*					Client Session						*/
	/********************************************************/
	void RequestCreateAndJoinSession(const FName& NewSessionName);		// 请求创建并加入会话
	void CancelCreateSession();		// 取消创建
	void StartGlobalSessionSearch();	// 开始全局会话搜索
	
	FOnJoinSessionFailedDelegate JoinSessionFailedDelegate;		// 加入会话失败委托
	FOnGlobalSessionSearchCompletedDelegate GlobalSessionSearchCompletedDelegate;		// 全局会话搜索完成委托
	
private:
	void OnCreateAndJoinSessionResponseReceived(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse, bool bConnectedSuccessful, FName SessionName, FGuid SessionSearchId);
	void StartFindCreatedSession(const FGuid& SessionSearchId);		// 开始查找已创建的会话
	void StopAllSessionFindings();		// 停止所有会话查找
	void StopFindingCreatedSession();		// 停止查找已创建的会话
	void StopGlobalSessionFindings();		// 停止全局会话查找

	void OnGlobalSessionSearchCompleted(bool bWasSuccessful);		// 全局会话搜索完成回调
	void PerformGlobalSessionSearch();		// 执行全局会话搜索
	
	FTimerHandle FindCreatedSessionTimerHandle;				// 查找已创建会话定时器句柄（用于规定间隔多久执行一次定期会话查找）
	FTimerHandle FindCreatedSessionTimeoutTimerHandle;		// 查找已创建会话超时定时器句柄（用于规定查找已创建会话的总时长）
	FTimerHandle GlobalSessionSearchTimerHandle;			// 全局会话搜索定时器句柄
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float FindCreatedSessionInterval = 1.f;			// 查找已创建会话间隔时长（秒）
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float FindCreatedSessionTimeoutDuration = 60.f;			// 查找已创建会话超时时长（秒）
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float GlobalSessionSearchInterval = 2.f;			// 全局会话搜索间隔时长（秒）

	void FindCreatedSessionComplete(bool bWasSuccessful);		// 查找已创建会话完成回调
	void FindCreatedSession(FGuid SessionSearchId);
	void FindCreatedSessionTimeout();
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type JoinSessionResult, int Port);		// 加入会话完成回调
	void JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult);		// 使用搜索结果加入会话
	
	TSharedPtr<FOnlineSessionSearch> CurrentSessionSearch;		// 当前会话搜索对象指针
	
private:
	/********************************************************/
	/*					Session Server						*/
	/********************************************************/
	
	// 会话创建完成回调
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
	void CreateSession();		// 创建会话
	
	FString ServerSessionName;		// 服务器会话名称
	int SessionServerPort;			// 服务器会话端口
	FTimerHandle WaitPlayerJoinSessionTimeoutHandle;		// 等待玩家加入会话超时句柄
	
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeoutDuration = 60.f;				// 等待玩家加入会话超时时长（秒）
	
	void WaitPlayerJoinSessionTimeout();		// 等待玩家加入会话超时处理函数

	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful); // 结束会话完成回调
	void TerminateSessionServer();		// 终止会话服务
	
	TSet<FUniqueNetIdRepl> PlayerJoinedSessions;		// 已加入会话的玩家列表
	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevelMap;

	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
};

