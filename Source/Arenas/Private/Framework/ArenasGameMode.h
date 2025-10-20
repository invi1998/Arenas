// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "ArenasGameMode.generated.h"

class AMinionBarrack;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamPlayerKillChangedSignature, FGenericTeamId, TeamID, int32, NewKillCount);

/**
 * 
 */
UCLASS()
class AArenasGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AArenasGameMode();

	virtual void BeginPlay() override;
	
	// 该函数会在玩家生成的时候被调用，无论是本地客户端还是通过网络远程客户端连接生成
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

	void RegisterMinionBarrack(const FGenericTeamId& InTeamID, AMinionBarrack* InBarrack);
	AMinionBarrack* GetBarrackByTeamID(const FGenericTeamId& InTeamID) const;

	void AddPlayerKillForTeam(const FGenericTeamId& InTeamID);
	int32 GetPlayerKillCountForTeam(const FGenericTeamId& InTeamID) const;

	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnTeamPlayerKillChangedSignature OnTeamPlayerKillChanged;

private:
	FGenericTeamId GetTeamIDFromPlayerController(const APlayerController* InPlayerController) const;

	AActor* FindNextStartingSpotForTeam(const FGenericTeamId& InTeamID) const;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TMap<FGenericTeamId, FName> TeamStartingSpotTagsMap; // 队伍起始点标签映射表，Key为队伍ID，Value为起始点标签

	UPROPERTY()
	TMap<FGenericTeamId, AMinionBarrack*> BarracksMap;

	UPROPERTY()
	TMap<FGenericTeamId, int32> TeamPlayerKillCountMap;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TSet<FGenericTeamId> ValidTeamIDs;
	
};
