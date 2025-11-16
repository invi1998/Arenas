// Ace of Arenas. (invi_1998 All Rights Reserved)


#include "ArenasNetFunctionLibrary.h"

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

