// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArenasGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompletedDelegate, bool /*bWasSuccessful*/, const FString& /*PlayerNickName*/, const FString& /*Error*/);

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
