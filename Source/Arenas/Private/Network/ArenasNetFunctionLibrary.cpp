// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasNetFunctionLibrary.h"

IOnlineSessionPtr UArenasNetFunctionLibrary::GetSessionPtr()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return nullptr;
	return Subsystem->GetSessionInterface();
}

IOnlineIdentityPtr UArenasNetFunctionLibrary::GetIdentityPtr()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return nullptr;
	return Subsystem->GetIdentityInterface();
}

FOnlineSessionSettings UArenasNetFunctionLibrary::GenerateOnlineSessionSettings(const FName& SessionName, int Port,
	const FString& SessionSearchId)
{
	int NumPublicConnections = GetPlayerCounterPerTeam() * 2; // 每队玩家数量乘以2作为公共连接数
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false;									// 非局域网匹配
	SessionSettings.bUsesPresence = false;									// 不使用在线状态，因为我们是服务器创建会话，而服务器不需要登录状态
	SessionSettings.NumPublicConnections = NumPublicConnections;			// 公共连接数
	SessionSettings.NumPrivateConnections = 0;								// 私有连接数
	SessionSettings.bAllowJoinInProgress = false;							// 不允许中途加入
	SessionSettings.bAllowJoinViaPresence = false;							// 不允许通过在线状态加入
	SessionSettings.bAllowJoinViaPresenceFriendsOnly = false;				// 不允许仅通过在线状态的好友加入
	SessionSettings.bShouldAdvertise = true;								// 应该被广告宣传，即公开可见
	SessionSettings.bAllowInvites = true;									// 允许邀请
	SessionSettings.bIsDedicated = true;									// 专用服务器
	SessionSettings.bUseLobbiesIfAvailable = false;							// 不使用大厅功能
	SessionSettings.bUseLobbiesVoiceChatIfAvailable = false;				// 不使用大厅
	SessionSettings.bUsesStats = true;										// 使用统计数据(例如：胜率，成就等)

	// 自定义设置，ViaOnlineServiceAndPing表示该数据会被广告宣传，并且可以通过ping值进行过滤
	SessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(GetSessionPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return SessionSettings;
}

uint8 UArenasNetFunctionLibrary::GetPlayerCounterPerTeam()
{
	return 5;
}

bool UArenasNetFunctionLibrary::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject && WorldContextObject->GetWorld() && 
		WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer;
	
	// 如果是需要监听服务器（即玩家既是客户端又是服务器），则可以使用下面的代码
	//  WorldContextObject->GetWorld()->GetNetMode() == ENetMode::NM_ListenServer
}

FString UArenasNetFunctionLibrary::GetSessionNameStr()
{
	return GetCommandlineAreAsString(GetSessionNameKey());
}

FName UArenasNetFunctionLibrary::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UArenasNetFunctionLibrary::GetSessionSearchIdStr()
{
	return GetCommandlineAreAsString(GetSessionSearchIdKey());
}

FName UArenasNetFunctionLibrary::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UArenasNetFunctionLibrary::GetSessionPort()
{
	return GetCommandlineAreAsInt(GetSessionPortKey());
}

FName UArenasNetFunctionLibrary::GetSessionPortKey()
{
	return FName("PORT");
}

FString UArenasNetFunctionLibrary::GetCommandlineAreAsString(const FName& ParamName)
{
	FString OutVal = "";
	// 将参数名称转换为命令行参数格式（例如：-ParamName=）
	FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	// 从命令行中提取参数值
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UArenasNetFunctionLibrary::GetCommandlineAreAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *ParamName.ToString());
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

