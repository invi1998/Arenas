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

void UArenasGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UArenasNetFunctionLibrary::GetSessionPtr();
	if (SessionPtr.IsValid())
	{
		// 因为我们需要为多场游戏启动多个会话，所以需要一种方式在启动会话的时候告诉他们自己的名称以及应该使用哪个端口
		// 那样在玩家加入或者搜索的时候就能有一个唯一ID使用
		ServerSessionName = UArenasNetFunctionLibrary::GetSessionNameStr();
		FString SessionSearchId = UArenasNetFunctionLibrary::GetSessionSearchIdStr();
		SessionServerPort = UArenasNetFunctionLibrary::GetSessionPort();
		
		FOnlineSessionSettings OnlineSessionSettings = UArenasNetFunctionLibrary::GenerateOnlineSessionSettings(FName(*ServerSessionName), SessionServerPort, SessionSearchId);
		if (!SessionPtr->CreateSession(0, FName(*ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Error, TEXT("#### Failed to create session: %s"), *ServerSessionName);
			return;
		}
	
		UE_LOG(LogTemp, Warning, TEXT("#### Creating Session: %s on Port: %d with SearchId: %s"), *ServerSessionName, SessionServerPort, *SessionSearchId);
	
	}
	
	
	
}

void UArenasGameInstance::LoadLevelAndListen(const TSoftObjectPtr<UWorld>& LevelToLoad)
{
	// 获取关卡包名
	const FName LevelURL = FName(FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));

	if (!LevelURL.IsNone())
	{
		GetWorld()->ServerTravel(LevelURL.ToString() + "?listen");	// 加载关卡并监听
	}
	
}
