// Ace of Arenas. (invi_1998 All Rights Reserved)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ArenasNetFunctionLibrary.generated.h"

/**
 * 网络功能库
 */
UCLASS()
class ARENAS_API UArenasNetFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
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
	
};
