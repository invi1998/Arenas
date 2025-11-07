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


	
};
