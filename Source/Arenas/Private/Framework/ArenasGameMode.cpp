// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ArenasGameMode.h"

#include "ArenasGameState.h"
#include "EngineUtils.h"
#include "AI/MinionBarrack.h"
#include "DefenceTower/StormCore.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Network/ArenasGameSession.h"
#include "player/ArenasPlayerController.h"
#include "player/State/ArenasPlayerState.h"

AArenasGameMode::AArenasGameMode()
{
	// 设置游戏会话类
	GameSessionClass = AArenasGameSession::StaticClass();
}

void AArenasGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ArenasGameState = ArenasGameState ? ArenasGameState : Cast<AArenasGameState>(UGameplayStatics::GetGameState(this));
	
}

void AArenasGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AArenasGameMode::StartPlay()
{
	Super::StartPlay();

	
}

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

UClass* AArenasGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	AArenasPlayerState* ArenasPlayerState = InController ? InController->GetPlayerState<AArenasPlayerState>() : nullptr;
	if (ArenasPlayerState && ArenasPlayerState->GetSelectedPawnClass())
	{
		return ArenasPlayerState->GetSelectedPawnClass();
	}
	return BackupPawn;
}

APawn* AArenasGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	FGenericTeamId TeamID = GetTeamIDFromPlayerController(NewPlayer);
	if (IGenericTeamAgentInterface* NewPlayerTeamAgent = Cast<IGenericTeamAgentInterface>(NewPlayer))
	{
		
		NewPlayerTeamAgent->SetGenericTeamId(TeamID);
	}

	StartSpot = FindNextStartingSpotForTeam(TeamID);
	NewPlayer->StartSpot = StartSpot;		// 设置玩家控制器的起始点
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
	
}

void AArenasGameMode::RegisterMinionBarrack(const FGenericTeamId& InTeamID, AMinionBarrack* InBarrack)
{
	BarracksMap.Add(InTeamID, InBarrack);

	InBarrack->OnStormCoreDestroyed.AddUObject(this, &AArenasGameMode::MatchFinished);
	
}

AMinionBarrack* AArenasGameMode::GetBarrackByTeamID(const FGenericTeamId& InTeamID) const
{
	return BarracksMap.Contains(InTeamID) ? BarracksMap[InTeamID] : nullptr;
}

void AArenasGameMode::AddPlayerKillForTeam(const FGenericTeamId& InTeamID)
{
	if (ArenasGameState)
	{
		if (InTeamID == FGenericTeamId(0))
		{
			ArenasGameState->AddTeamOnePlayerKillCount();
		}
		else if (InTeamID == FGenericTeamId(1))
		{
			ArenasGameState->AddTeamTwoPlayerKillCount();
		}
	}
}

FGenericTeamId AArenasGameMode::GetTeamIDFromPlayerController(const AController* InController) const
{
	if (AArenasPlayerState* ArenasPlayerState = InController ? InController->GetPlayerState<AArenasPlayerState>() : nullptr)
	{
		// 通过玩家状态获取队伍ID
		return ArenasPlayerState->GetTeamIdBasedOnSlot();
	}
	
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

void AArenasGameMode::MatchFinished(AMinionBarrack* DestroyMinionBarrack)
{
	if (!DestroyMinionBarrack) return;
	// UCameraComponent* StormCoreView = DestroyMinionBarrack->GetStormCore()->GetStormCoreViewCam();
	AActor* ViewTarget = DestroyMinionBarrack->GetStormCore();
	FGenericTeamId LooseTeamID = DestroyMinionBarrack->GetBarrackTeamID();

	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AArenasPlayerController> It(World); It; ++It)
		{
			It->MatchFinished(ViewTarget, LooseTeamID);
		}
	}
}
