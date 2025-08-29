// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenasGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AArenasGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// 该函数会在玩家生成的时候被调用，无论是本地客户端还是通过网络远程客户端连接生成
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	
};
