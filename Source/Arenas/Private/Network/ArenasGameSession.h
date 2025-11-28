// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "ArenasGameSession.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinSessionDelegate, const FUniqueNetIdRepl& /*UniqueId*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerLeaveSessionDelegate, const FUniqueNetIdRepl& /*UniqueId*/);

UCLASS()
class ARENAS_API AArenasGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArenasGameSession();
	
	FOnPlayerJoinSessionDelegate OnPlayerJoinSessionDelegate;					// 玩家加入会话委托
	FOnPlayerLeaveSessionDelegate OnPlayerLeaveSessionDelegate;					// 玩家离开会话委托
	
	virtual bool ProcessAutoLogin() override;		// 处理自动登录
	
	// 注册和注销玩家（响应玩家加入和离开会话）
	virtual void RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite) override;
	virtual void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;

};
