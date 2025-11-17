// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasGameSession.h"
#include "Framework/ArenasGameInstance.h"

// Sets default values
AArenasGameSession::AArenasGameSession()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

bool AArenasGameSession::ProcessAutoLogin()
{
	// 仅在专用服务器上处理自动登录，因为我们的服务器不需要登录，所以这里直接返回true
	return true;
}

void AArenasGameSession::RegisterPlayer(APlayerController* NewPlayer, const FUniqueNetIdRepl& UniqueId, bool bWasFromInvite)
{
	Super::RegisterPlayer(NewPlayer, UniqueId, bWasFromInvite);
	if (UArenasGameInstance* ArenasGameInstance = GetGameInstance<UArenasGameInstance>())
	{
		// 玩家注册到会话后，清除等待玩家加入会话的超时定时器
		ArenasGameInstance->PlayerJoinedSession(UniqueId);
	}
}

void AArenasGameSession::UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId)
{
	Super::UnregisterPlayer(InSessionName, UniqueId);
	if (UArenasGameInstance* ArenasGameInstance = GetGameInstance<UArenasGameInstance>())
	{
		ArenasGameInstance->PlayerLeftSession(UniqueId);
	}
}


