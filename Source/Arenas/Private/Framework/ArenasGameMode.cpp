// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ArenasGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* AArenasGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	if (IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(NewPlayerController))
	{
		// 为新生成的玩家控制器分配一个队伍ID
		FGenericTeamId TeamID = GetTeamIDFromPlayerController(NewPlayerController);
		TeamAgent->SetGenericTeamId(TeamID);

		// 为新生成的玩家控制器分配一个起始点
		NewPlayerController->StartSpot = FindNextStartingSpotForTeam(TeamID);
	}
	
	return NewPlayerController;
}

FGenericTeamId AArenasGameMode::GetTeamIDFromPlayerController(const APlayerController* InPlayerController) const
{
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2); // 简单地将玩家分配到两个队伍
}

AActor* AArenasGameMode::FindNextStartingSpotForTeam(const FGenericTeamId& InTeamID) const
{
	const FName* StartingSpotTag = TeamStartingSpotTagsMap.Find(InTeamID);
	if (StartingSpotTag && !StartingSpotTag->IsNone())
	{
		if (UWorld* World = GetWorld())
		{
			for (TActorIterator<APlayerStart> It(World); It; ++It)
			{
				if (It->PlayerStartTag == *StartingSpotTag)
				{
					It->PlayerStartTag = FName("Taken"); // 标记该起始点已被占用，防止多个玩家生成在同一位置
					return *It;
				}
			}
		}
	}

	return nullptr;
}
