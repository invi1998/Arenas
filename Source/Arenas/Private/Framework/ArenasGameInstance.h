// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArenasGameInstance.generated.h"

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

	
/********************************************************/
/*					Session Server						*/
/********************************************************/
private:
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
	

private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevelMap;

	void LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad);
	
};
