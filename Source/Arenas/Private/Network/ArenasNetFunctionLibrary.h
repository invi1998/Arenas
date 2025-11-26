// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineServices.h"

#include "ArenasNetFunctionLibrary.generated.h"

/**
 * 网络功能库
 */
UCLASS()
class ARENAS_API UArenasNetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static IOnlineSessionPtr GetSessionPtr();		// 获取在线会话对象指针
	static IOnlineIdentityPtr GetIdentityPtr();	// 获取在线身份对象指针
	static UE::Online::IOnlineServicesPtr GetOnlineServicesPtr(); // 获取在线服务对象指针
	
	// 生成在线会话设置
	static FOnlineSessionSettings GenerateOnlineSessionSettings(const FName& SessionName, int Port, const FString& SessionSearchId);
	
	static uint8 GetPlayerCounterPerTeam();		// 获取每个队伍的玩家数量

	static bool IsSessionServer(const UObject* WorldContextObject);			// 是否为会话服务器
	
	static FString GetSessionNameStr();			// 获取会话名称字符串
	static FName GetSessionNameKey();			// 获取会话名称键
	
	static FString GetSessionSearchIdStr();		// 获取会话搜索ID字符串
	static FName GetSessionSearchIdKey();		// 获取会话搜索ID键
	
	static int GetSessionPort();			// 获取会话端口
	static FName GetSessionPortKey();				// 获取会话端口键
	
	static FString GetCommandlineAreAsString(const FName& ParamName);		// 获取命令行参数字符串值
	static int GetCommandlineAreAsInt(const FName& ParamName);			// 获取命令行参数整数值
	
	// 协调器URL，用于注册和发现会话服务器
	static FName GetCoordinatorURLKey();			// 获取协调器URL键
	static FString GetCoordinatorURL();				// 获取协调器URL字符串值
	static FString GetDefaultCoordinatorURL();		// 获取默认协调器URL字符串值
	
	static FString GetTestingURL();				// 获取测试URL字符串值
	static FName GetTestingURLKey();			// 获取测试URL键
	
	static void ReplacePortInURL(FString& InOutURL, int NewPort);		// 替换URL中的端口号
	
};
